"use strict";

/*
 * anyboard-cypawn-token
 *
 * Driver for communicating with LightBlue cypawn hardware (http://legacy.punchthrough.com/cypawn/)
 * Supports cypawnToken.ino and cypawnPrinter.ino firmware
 *
 * Dependencies:
 *      AnyBoard (...)
 *      cordova.js (https://cordova.apache.org/)
 *      evothings.ble (https://github.com/evothings/cordova-ble)
 *      evothings.easyble (https://github.com/evothings/evothings-examples/blob/master/resources/libs/evothings/easyble/easyble.js)
 */

(function(){
    var cypawnBluetooth = new AnyBoard.Driver({
        name: 'anyboard-cypawn-token',
        description: 'Driver based off evothings.easyble library for Cordova-based apps',
        dependencies: 'evothings.easyble',
        version: '0.1',
        date: '2015-08-01',
        type: ['bluetooth'],
        compatibility: [
            {
                descriptor_uuid: '00002902-0000-1000-8000-00805f9b34fb',			//Characteristic config
                characteristic_uuid: '5846d509-8694-44a2-8d6f-fe7d06bc560d',		//Serial
                service_uuid: '6fe71585-adfe-41dd-9b02-8f984d40f654'				//CyPawn AnyBoardJS
            }
        ]
    });

    cypawnBluetooth._devices = {};

    /*
     * Internal method that generates a send function.
     * @param {string} name name of functionality. This is what will be used as cache entry, in logs,
     * @param {number} functionId The integer representation of the function, which will be sent as a command to
     *      the token. For instance, ledOn is 129.
     * @param {boolean} [hasParams=false] Whether or not this function accepts data to be sent to the token.
     *      For instance, ledOn takes parameters for color, while ledOff does not.
     * @param {boolean} [useCache=false] Whether or not cache should be used in this function.
     *      If cache is used, function will not send more than one request, no matter how many
     *      times this function is called. Used for getting token name and functionality (not changing pr token)
     * @returns {Function} A function that can be called to send a command to the token
     * @private
     */
    cypawnBluetooth._GenericSend = function(name, functionId, hasParams, useCache) {
        var internalSend = function(token, data, win, fail) {
            AnyBoard.Logger.debug("Executing " + name, token);
            if (useCache && token.cache[name]) {
                win && win(token.cache[name]);
                return;
            }
            cypawnBluetooth.send(
                token,
                data,
                function()
				{
                    token.once(name,
					function(returnData)
					{
						if (useCache)
						{
							token.cache[name] = returnData;
						}
						win && win(returnData);
					}
                    )
                },
                function(errorCode){ fail && fail(errorCode);}
            )
        };
        if (!hasParams) {
            var data = new Uint8Array(1);
            data[0] = functionId;
            return function(token, win, fail) {
                internalSend(token, data, win, fail);
            };
        }
        return function(token, data, win, fail) {
            if (typeof data === 'string') data = cypawnBluetooth._stringToUint8(data);
            var newData = new Uint8Array(data.length+1);
            newData[0] = functionId;
            for (var index in data) {
                if (data.hasOwnProperty(index))
                    newData[parseInt(index)+1] = data[index];
            }
            internalSend(token, newData, win, fail);
        }
    };

    /* Internal mapping from commands to uint8 representations */
    cypawnBluetooth._CMD_CODE = {
        MOVE: 194,
        GET_NAME: 32,
        GET_VERSION: 33,
        GET_UUID: 34,
        GET_BATTERY_STATUS: 35,
        GET_COLOR: 37,
        LED_OFF: 128,
        LED_ON: 129,
        LED_BLINK: 130,
        HAS_LED: 64,
        HAS_LED_COLOR: 65,
        HAS_VIBRATION: 66,
        HAS_COLOR_DETECTION: 67,
        HAS_LED_SCREEN: 68,
        HAS_RFID: 71,
        HAS_NFC: 72,
        HAS_ACCELEROMETER: 73,
        HAS_TEMPERATURE: 74,
        HAS_PRINT: 75,
        PRINT_FEED: 137,
        PRINT_JUSTIFY: 138,
        PRINT_SET_SIZE: 139,
        PRINT_WRITE: 140,
        PRINT_NEWLINE: 141
    };


    /* Internal mapping between color strings to Uint8 array of RGB colors */
    cypawnBluetooth._COLORS = {
        'red': new Uint8Array([255, 0, 0]),
        'green': new Uint8Array([0, 255, 0]),
        'blue': new Uint8Array([0, 0, 255]),
        'white': new Uint8Array([255, 255, 255]),
        'pink': new Uint8Array([255, 0, 255]),
        'yellow': new Uint8Array([255, 255, 0]),
        'aqua': new Uint8Array([0, 255, 255]),
        'off': new Uint8Array([0, 0, 0])
    };

    /* Internal mapping and generation of commands */
    var NO_PARAMS = false;
    var HAS_PARAMS = true;
    var USE_CACHE = true;
    cypawnBluetooth._COMMANDS = {
        GET_NAME: cypawnBluetooth._GenericSend(
            "GET_NAME",
            cypawnBluetooth._CMD_CODE.GET_NAME,
            NO_PARAMS,
            USE_CACHE),
        GET_VERSION: cypawnBluetooth._GenericSend(
            "GET_VERSION",
            cypawnBluetooth._CMD_CODE.GET_VERSION,
            NO_PARAMS,
            USE_CACHE),
        GET_UUID: cypawnBluetooth._GenericSend(
            "GET_UUID",
            cypawnBluetooth._CMD_CODE.GET_UUID,
            NO_PARAMS,
            USE_CACHE),
        GET_BATTERY_STATUS: cypawnBluetooth._GenericSend(
            "GET_BATTERY_STATUS",
            cypawnBluetooth._CMD_CODE.GET_BATTERY_STATUS,
            NO_PARAMS),
        LED_OFF: cypawnBluetooth._GenericSend(
            "LED_OFF",
            cypawnBluetooth._CMD_CODE.LED_OFF,
            NO_PARAMS),
        LED_ON: cypawnBluetooth._GenericSend(
            "LED_ON",
            cypawnBluetooth._CMD_CODE.LED_ON,
            HAS_PARAMS),
        LED_BLINK: cypawnBluetooth._GenericSend(
            "LED_BLINK",
            cypawnBluetooth._CMD_CODE.LED_BLINK,
            HAS_PARAMS),
        HAS_LED: cypawnBluetooth._GenericSend(
            "HAS_LED",
            cypawnBluetooth._CMD_CODE.HAS_LED,
            NO_PARAMS,
            USE_CACHE),
        HAS_LED_COLOR: cypawnBluetooth._GenericSend(
            "HAS_LED_COLOR",
            cypawnBluetooth._CMD_CODE.HAS_LED_COLOR,
            NO_PARAMS,
            USE_CACHE),
        HAS_VIBRATION: cypawnBluetooth._GenericSend(
            "HAS_VIBRATION",
            cypawnBluetooth._CMD_CODE.HAS_VIBRATION,
            NO_PARAMS,
            USE_CACHE),
        HAS_COLOR_DETECTION: cypawnBluetooth._GenericSend(
            "HAS_COLOR_DETECTION",
            cypawnBluetooth._CMD_CODE.HAS_COLOR_DETECTION,
            NO_PARAMS,
            USE_CACHE),
        HAS_LED_SCREEN: cypawnBluetooth._GenericSend(
            "HAS_LED_SCREEN",
            cypawnBluetooth._CMD_CODE.HAS_LED_SCREEN,
            NO_PARAMS,
            USE_CACHE),
        HAS_RFID: cypawnBluetooth._GenericSend(
            "HAS_RFID",
            cypawnBluetooth._CMD_CODE.HAS_RFID,
            NO_PARAMS,
            USE_CACHE),
        HAS_NFC: cypawnBluetooth._GenericSend(
            "HAS_NFC",
            cypawnBluetooth._CMD_CODE.HAS_NFC,
            NO_PARAMS,
            USE_CACHE),
        HAS_ACCELEROMETER: cypawnBluetooth._GenericSend(
            "HAS_ACCELEROMETER",
            cypawnBluetooth._CMD_CODE.HAS_ACCELEROMETER,
            NO_PARAMS,
            USE_CACHE),
        HAS_TEMPERATURE: cypawnBluetooth._GenericSend(
            "HAS_TEMPERATURE",
            cypawnBluetooth._CMD_CODE.HAS_TEMPERATURE,
            NO_PARAMS,
            USE_CACHE),
        HAS_PRINT: cypawnBluetooth._GenericSend(
            "HAS_PRINT",
            cypawnBluetooth._CMD_CODE.HAS_PRINT,
            NO_PARAMS,
            USE_CACHE),
        PRINT_FEED: cypawnBluetooth._GenericSend(
            "PRINT_FEED",
            cypawnBluetooth._CMD_CODE.PRINT_FEED,
            NO_PARAMS),
        PRINT_JUSTIFY: cypawnBluetooth._GenericSend(
            "PRINT_JUSTIFY",
            cypawnBluetooth._CMD_CODE.PRINT_JUSTIFY,
            HAS_PARAMS),
        PRINT_SET_SIZE: cypawnBluetooth._GenericSend(
            "PRINT_SET_SIZE",
            cypawnBluetooth._CMD_CODE.PRINT_SET_SIZE,
            HAS_PARAMS),
        PRINT_WRITE: cypawnBluetooth._GenericSend(
            "PRINT_WRITE",
            cypawnBluetooth._CMD_CODE.PRINT_WRITE,
            HAS_PARAMS),
        PRINT_NEWLINE: cypawnBluetooth._GenericSend(
            "PRINT_NEWLINE",
            cypawnBluetooth._CMD_CODE.PRINT_NEWLINE,
            NO_PARAMS),
        GET_COLOR: cypawnBluetooth._GenericSend(
            "GET_COLOR",
            cypawnBluetooth._CMD_CODE.GET_COLOR,
            NO_PARAMS)
    };

    cypawnBluetooth.getName = function (token, win, fail) {
        this._COMMANDS.GET_NAME(token, win, fail);
    };

    cypawnBluetooth.getVersion = function (token, win, fail) {
        this._COMMANDS.GET_VERSION(token, win, fail);
    };

    cypawnBluetooth.getUUID = function (token, win, fail) {
        this._COMMANDS.GET_UUID(token, win, fail);
    };

    cypawnBluetooth.hasLed = function(token, win, fail) {
        this._COMMANDS.HAS_LED(token, win, fail);
    };

    cypawnBluetooth.hasLedColor = function(token, win, fail) {
        this._COMMANDS.HAS_LED_COLOR(token, win, fail);
    };

    cypawnBluetooth.hasVibration = function(token, win, fail) {
        this._COMMANDS.HAS_VIBRATION(token, win, fail);
    };

    cypawnBluetooth.hasColorDetection = function(token, win, fail) {
        this._COMMANDS.HAS_COLOR_DETECTION(token, win, fail);
    };

    cypawnBluetooth.hasLedSceen = function(token, win, fail) {
        this._COMMANDS.HAS_LED_SCREEN(token, win, fail);
    };

    cypawnBluetooth.hasRfid = function(token, win, fail) {
        this._COMMANDS.HAS_RFID(token, win, fail);
    };

    cypawnBluetooth.hasNfc = function(token, win, fail) {
        this._COMMANDS.HAS_NFC(token, win, fail);
    };

    cypawnBluetooth.hasAccelometer = function(token, win, fail) {
        this._COMMANDS.HAS_ACCELEROMETER(token, win, fail);
    };

    cypawnBluetooth.hasTemperature = function(token, win, fail) {
        this._COMMANDS.HAS_TEMPERATURE(token, win, fail);
    };

    cypawnBluetooth.ledOn = function (token, value, win, fail) {
       value = value || 'white';

        if (typeof value === 'string' && value in this._COLORS) {
            this._COMMANDS.LED_ON(token, this._COLORS[value], win, fail);
        } else if ((value instanceof Array || value instanceof Uint8Array) && value.length === 3) {
            this._COMMANDS.LED_ON(token, new Uint8Array([value[0], value[1], value[2]]), win, fail);
        } else {
            fail && fail('Invalid or unsupported color parameters');
        }
	
    };
	
	
    cypawnBluetooth.getColor = function (token, win, fail) {
        this._COMMANDS.GET_COLOR(token, win, fail);
	
    };

    cypawnBluetooth.ledBlink = function (token, value, win, fail) {
        value = value || 'white';

        if (typeof value === 'string' && value in this._COLORS) {
            cypawnBluetooth.ledBlink(token, this._COLORS[value], win, fail);
        } else if ((value instanceof Array || value instanceof Uint8Array) && value.length === 3) {
            this._COMMANDS.LED_BLINK(token, new Uint8Array([value[0], value[1], value[2], 30]), win, fail);
        } else {
            fail && fail('Invalid or unsupported color parameters');
        }
    };

    cypawnBluetooth.ledOff = function (token, win, fail) {
        this._COMMANDS.LED_OFF(token, win, fail);
    };

    cypawnBluetooth.hasPrint = function (token, win, fail) {
        COMMANDS.HAS_PRINT(token, win, fail);
    };

    /**
     * Internal method. Raw sending of data to cypawn. Does not employ throttling/queueing, nor validates send data.
     *
     * See https://github.com/PunchThrough/cypawn-documentation/blob/master/app_message_types.md
     * and https://github.com/PunchThrough/cypawn-documentation/blob/master/serial_message_protocol.md
     * for information on cypawn spesific BLE protocol
     *
     * @param {AnyBoard.BaseToken} token token that should be sent to
     * @param {Uint8Array} data data that should be sent to token
     * @param {Function} win function to be called upon success of sending data
     * @param {Function} fail function to be called upon failure of sending data
     */
    cypawnBluetooth.rawSend = function(token, data, win, fail) {
        evothings.ble.writeCharacteristic(
            token.device.deviceHandle,
            token.device.serialChar,
            data,
            win,
            fail
        );
    };

    /**
     * Sends data to device. Uses throttling.
     *
     * @param {AnyBoard.BaseToken} token token to send data to
     * @param {ArrayBuffer|Uint8Array|String} data data to be sent (max 13 byte)
     * @param {function} [win] function to be executed upon success
     * @param {function} [fail] function to be executed upon failure
     */
    cypawnBluetooth.send = function(token, data, win, fail) {
        var self = this;

        if(!(token.device.haveServices)) {
            fail && fail('Token does not have services');
            return;
        }

        if (typeof data === 'string') {
            data = new Uint8Array(evothings.ble.toUtf8(data));
        }

        if(data.buffer) {
            if(!(data instanceof Uint8Array))
                data = new Uint8Array(data.buffer);
        } else if(data instanceof ArrayBuffer) {
            data = new Uint8Array(data);
        } else {
            AnyBoard.Logger.warn("send data is not an ArrayBuffer.", this);
            return;
        }

        if (data.length > 20) {
            AnyBoard.Logger.warn("cannot send data of length over 20.", this);
            return;
        }

        if (token.sendQueue.length === 0) {  // this was first command
            token.sendQueue.push(function(){ cypawnBluetooth.rawSend(token, data, win, fail); });
            cypawnBluetooth.rawSend(token, data, win, fail);
        } else {
            // send function will be handled by existing
            token.sendQueue.push(function(){ cypawnBluetooth.rawSend(token, data, win, fail); });

            // Disregards existing queue if it takes more than 2000ms
            var randomToken = Math.random();
            token.randomToken = randomToken;

            setTimeout(function() {
                if (token.randomToken == randomToken) { // Queuehandler Hung up

                    token.sendQueue.shift(); // Remove function from queue
                    if (token.sendQueue.length > 0) {  // If there's more functions queued
                        token.sendQueue[0]();  // Send next function off
                    }
                }
            }, 2000);
        }

    };

    /**
     * Internal method that subscribes to updates from the token
     * @param {AnyBoard.Basetoken} token token that should be subscribed to
     * @param {Function} callback function to be executed once we receive updates 
     * @param {Function} success function to be executed if we manage to subscribe to updates
     * @param {Function} fail function to be executed if we fail to subscribe to updates
     * @private
     */
    cypawnBluetooth._subscribe = function(token, callback, success, fail) {
        evothings.ble.writeDescriptor(
            token.device.deviceHandle,
            token.device.serialDesc,
            new Uint8Array([1, 0]),
            function (data) {
                AnyBoard.Logger.log("successfully subscribed to notifications from", token);
                success && success();
            },
            function (data) {
                AnyBoard.Logger.log("failed at subscribing to notifications from", token);
				AnyBoard.Logger.log("Descriptor : " + token.device.serialDesc, token);
                fail && fail();
            }
        );

        evothings.ble.enableNotification(token.device.deviceHandle, token.device.serialChar, function (data) {
            data = new Uint8Array(data);
            /*var gtHeader = data[0];

            // The update packet is one in a series of packets
            if ((gtHeader & 0x9f) != 0x80) {
                cypawnBluetooth._handleMultiGST(token, data, gtHeader, callback);
                return;
            }

            // Unless an update packet, packets have a 7 byte overhead. Less than 8 has no data in other words,
            // and would be invalid.
            if (data.byteLength < 8) {
                AnyBoard.Logger.log("ignoring GT packet with bad length: " + data.byteLength, token);
                return;
            }

            var length = data[1];
            var majorId = data[3];
            var minorId = data[4];

            // Packet has wrong length compared to what it was sent as
            if (length != data.byteLength - 5) {
                AnyBoard.Logger.warn("ignoring incoming serial msg with bad length: " + length, token);
                return;
            }

            // majorId and minorId = 0 signals that this is a serial sent packet (as we want)
            if (majorId != 0 || minorId != 0) {
                AnyBoard.Logger.log("Ignoring incoming serial msg with unknown ID " + data, token);
                return;
            }

            // TODO: The Cycle Redundency Check value fails, and I don't know why.
            //var crc = cypawnBluetooth._computeCRC16(data, 1, length + 2);
            //if(data[data.byteLength-1] != ((crc >> 8) & 0xff) || data[data.byteLength-2] != (crc & 0xff)) {
            //    AnyBoard.Logger.log("ignoring GST message with bad CRC (our crc "+crc.toString(16)+", data "+cypawn.bytesToHexString(data, 1, length+2)+")");
            //    return;
            //}
			
            callback(data.subarray(5, data.byteLength - 2));
			*/
			
			callback(data);
			
        }, function (error) {
            AnyBoard.Logger.warn("Failed at parsing incoming serial message: " + error, token)
        });
    };

    /*
     * Interal method that handles receiving packets on updates that are not the first one.
     */
    cypawnBluetooth._handleMultiGST = function(token, data, gtHeader, callback) {
        var counter = gtHeader & 0x60;
        var remain = gtHeader & 0x1f;
        var first = (gtHeader & 0x80) == 0x80;
        var length = data[1];

        if(first) {
            if(length > 66) {
                AnyBoard.Logger.log("ignoring GST packet with bad length: "+length);
                return;
            }

            token._gstBuffer = new Uint8Array(length+4);
            // copy data, after GT header, length byte and Reserved byte, into buffer.
            token._gstBuffer.set(data.subarray(1));
            token._gstPosition = data.byteLength - 1;
            token._gstCounter = counter;
        } else {
            if(!token._gstBuffer) {
                AnyBoard.Logger.log("second packet received before first!");
                return;
            }
            if(counter != token._gstCounter) {
                AnyBoard.Logger.log("GT counter mismatch!");
                return;
            }
            if(remain != token._gstRemain - 1) {
                AnyBoard.Logger.log("GT remain mismatch!");
                return;
            }
            // copy data after GT header into the unwritten part of gstBuffer.
            token._gstBuffer.subarray(token._gstPosition).set(data.subarray(1));
            token._gstPosition += data.byteLength - 1;
        }
        token._gstRemain = remain;
        if(remain == 0) {
            data = token._gstBuffer;
            // check message id
            if(data[2] != 0 || data[3] != 0) {
                AnyBoard.Logger.log("ignoring App message with unknown ID " + cypawn.bytesToHexString(data, 2, 2));
                return;
            }

            /* TODO: Cycle redundancy check (validating integrety of data) fails, and I don't know why */
            //var crc = cypawnBluetooth._computeCRC16(data, 0, data.byteLength-2);
            //if(data[data.byteLength-1] != ((crc >> 8) & 0xff) || data[data.byteLength-2] != (crc & 0xff)) {
            //    AnyBoard.Logger.log("ignoring GST message with bad CRC (our crc "+crc.toString(16)+", data "+cypawn.bytesToHexString(data)+")");
            //    return;
            //}
            token._gstBuffer = false;
            callback(data.subarray(4, data.byteLength-2));

        }
    };

    /**
     * The initialize-methods is called automatically (or should be) from the master communication driver upon connect
     * to a device.
     *
     * In this initialize method, we subscribe to notifications sent by the cypawn device, and trigger events
     * on the token class upon receiving data.
     * @param {AnyBoard.BaseToken} token token instance to be initialized
     */
    cypawnBluetooth.initialize = function(token) {
        var handleReceiveUpdateFromToken = function(uint8array) {
            var command = uint8array[0];
            var stringData = "";
			
			AnyBoard.Logger.debug("Received command " + command, token);
			
            switch (command) {
                case cypawnBluetooth._CMD_CODE.GET_BATTERY_STATUS:
                    for (var i = 1; i < uint8array.length; i++)
                        stringData += String.fromCharCode(uint8array[i])
                    token.trigger('GET_BATTERY_STATUS', {"value": stringData});
                    break;
                case cypawnBluetooth._CMD_CODE.MOVE:
                    token.trigger('MOVE', {"value": uint8array[1], "newTile": uint8array[1], "oldTile": uint8array[2]});
                    break;
                case cypawnBluetooth._CMD_CODE.GET_NAME:
                    for (var i = 1; i < uint8array.length; i++)
                        stringData += String.fromCharCode(uint8array[i]);
                    token.trigger('GET_NAME', {"value": stringData});
                    break;
                case cypawnBluetooth._CMD_CODE.GET_VERSION:
                    for (var i = 1; i < uint8array.length; i++)
                        stringData += String.fromCharCode(uint8array[i]);
                    token.trigger('GET_VERSION', {"value": stringData});
                    break;
                case cypawnBluetooth._CMD_CODE.GET_UUID:
                    for (var i = 1; i < uint8array.length; i++)
                        stringData += String.fromCharCode(uint8array[i]);
                    token.trigger('GET_UUID', {"value": stringData});
                    break;
                case cypawnBluetooth._CMD_CODE.LED_BLINK:
                    token.trigger('LED_BLINK');
                    break;
                case cypawnBluetooth._CMD_CODE.LED_OFF:
                    token.trigger('LED_OFF');
                    break;
                case cypawnBluetooth._CMD_CODE.LED_ON:
                    token.trigger('LED_ON');
                    break;
                case cypawnBluetooth._CMD_CODE.HAS_LED:
                    token.trigger('HAS_LED', {"value": uint8array[1]});
                    break;
                case cypawnBluetooth._CMD_CODE.HAS_LED_COLOR:
                    token.trigger('HAS_LED_COLOR', {"value": uint8array[1]});
                    break;
                case cypawnBluetooth._CMD_CODE.HAS_VIBRATION:
                    token.trigger('HAS_VIBRATION', {"value": uint8array[1]});
                    break;
                case cypawnBluetooth._CMD_CODE.HAS_COLOR_DETECTION:
                    token.trigger('HAS_COLOR_DETECTION', {"value": uint8array[1]});
                    break;
                case cypawnBluetooth._CMD_CODE.HAS_LED_SCREEN:
                    token.trigger('HAS_LED_SCREEN', {"value": uint8array[1]});
                    break;
                case cypawnBluetooth._CMD_CODE.HAS_RFID:
                    token.trigger('HAS_RFID', {"value": uint8array[1]});
                    break;
                case cypawnBluetooth._CMD_CODE.HAS_NFC:
                    token.trigger('HAS_NFC', {"value": uint8array[1]});
                    break;
                case cypawnBluetooth._CMD_CODE.HAS_ACCELEROMETER:
                    token.trigger('HAS_ACCELEROMETER', {"value": uint8array[1]});
                    break;
                case cypawnBluetooth._CMD_CODE.HAS_TEMPERATURE:
                    token.trigger('HAS_TEMPERATURE', {"value": uint8array[1]});
                    break;
                case cypawnBluetooth._CMD_CODE.HAS_PRINT:
                    token.trigger('HAS_PRINT', {"value": uint8array[1]});
                    break;
                case cypawnBluetooth._CMD_CODE.PRINT_FEED:
                    token.trigger('PRINT_FEED');
                    break;
                case cypawnBluetooth._CMD_CODE.PRINT_JUSTIFY:
                    token.trigger('PRINT_JUSTIFY');
                    break;
                case cypawnBluetooth._CMD_CODE.PRINT_SET_SIZE:
                    token.trigger('PRINT_SET_SIZE');
                    break;
                case cypawnBluetooth._CMD_CODE.PRINT_WRITE:
                    token.trigger('PRINT_WRITE');
                    break;
                case cypawnBluetooth._CMD_CODE.PRINT_NEWLINE:
                    token.trigger('PRINT_NEWLINE');
                    break;
                case cypawnBluetooth._CMD_CODE.GET_COLOR:
                    for (var i = 0; i < uint8array.length; i++)
                        stringData += String.fromCharCode(uint8array[i]);
					 AnyBoard.Logger.log(stringData);
                    token.trigger('GET_COLOR', {"value": stringData});
                    break;
                default:
                    token.trigger('INVALID_DATA_RECEIVE', {"value": uint8array});
            }

            token.sendQueue.shift(); // Remove function from queue
            if (token.sendQueue.length > 0) {  // If there's more functions queued
                token.randomToken = Math.random();
                token.sendQueue[0]();  // Send next function off
            }
        };

        this._subscribe(token, handleReceiveUpdateFromToken);
    };

    /* Internal method that calculates redundancycheck value of incoming packets */
    cypawnBluetooth._computeCRC16 = function(data) {
        var crc = 0xFFFF;

        for (var i=0; i<data.length; i++) {
            var byte = data[i];
            crc = (((crc >> 8) & 0xff) | (crc << 8)) & 0xFFFF;
            crc ^= byte;
            crc ^= ((crc & 0xff) >> 4) & 0xFFFF;
            crc ^= ((crc << 8) << 4) & 0xFFFF;
            crc ^= (((crc & 0xff) << 4) << 1) & 0xFFFF;
        }

        return crc;
    };

    /*
     * Internal method that returns an object representing a GT message buffer.
     * @param {AnyBoard.BaseToken} token
     * @param {number} gstPacketLength
     * @returns {{packetCount: number, buf: Uint8Array, append: Function, packet: Function}}
     * @private
     */
    cypawnBluetooth._gtBuffer = function(token, gstPacketLength) {
        // BLE max is 20. GT header takes 1 byte.
        var packetCount = Math.ceil(gstPacketLength / 19);

        // We'll store all the packets in one buffer.
        var bufferLength = gstPacketLength + packetCount;

        var buf = new Uint8Array(bufferLength);
        var pos = 0;

        return {
            packetCount: packetCount,
            buf: buf,
            append: function(b) {
                // If this is the start of a GT packet, add the GT header.
                if ((pos % 20) == 0) {

                    // Decrement the local packetCount. This should not affect the member packetCount.
                    buf[pos] = cypawnBluetooth._gtHeader(token, --packetCount, pos);
                    pos++;
                }
                buf[pos++] = b;
            },
            // Returns the i:th packet in the message.
            packet: function(i) {
				var result = buf.subarray(i*20, Math.min((i+1)*20, bufferLength));
				for(var j = 0; j < result.length; j++)
					AnyBoard.Logger.log("GT_Packet : " + result[j]);
                return result;
            }
        };
    };

    /*
     * Returns the next GT header, given the number of packets remaining in the message.
     * @private
     */
    cypawnBluetooth._gtHeader = function(token, remain, pos) {
        var h = token.device.sendGtHeader + (remain);
        if (remain == 0) {
            token.device.sendGtHeader = (token.device.sendGtHeader + 0x20) & 0xff;
            if (token.device.sendGtHeader < 0x80) token.device.sendGtHeader = 0x80;
        }
        if (pos != 0) h &= 0x7f;
        return h;
    };

    /**
     * Converts a string to an Uint8Array.
     * @param string string to be converted
     * @returns {Uint8Array}
     * @private
     */
    cypawnBluetooth._stringToUint8 = function(string) {
        return new Uint8Array(evothings.ble.toUtf8(string));
    };

})();
