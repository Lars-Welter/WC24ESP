"use strict";
const iro = window.iro; // require("@jaames/iro");
(function(window, document) {

	let layout = document.querySelector(".layout");
	let menu = document.querySelector(".menu");
	let menuLink = document.querySelector(".hamburger");

	function toggleMenuEventHandler(event) {
		// It's a link, so don't activate the link but do some JS instead
		event.preventDefault();

		const active = "active";
		const menuIsOpen = menuLink.getAttribute("aria-expanded") === "true";

		if (menuIsOpen) {
			// Close menu
			console.log("is open, closing");
			menuLink.setAttribute("aria-expanded", "false");
			menuLink.setAttribute("aria-label", i18next.t("menu.aria-show-menu"));
			layout.classList.remove(active);
			menu.classList.remove(active);
		} else {
			console.log("is closed, opening");
			// Open menu
			menuLink.setAttribute("aria-expanded", "true");
			menuLink.setAttribute("aria-label", i18next.t("menu.aria-hide-menu"));
			layout.classList.add(active);
			menu.classList.add(active);
		}
	}

	if (menuLink) {
		menuLink.addEventListener("click", toggleMenuEventHandler);
	}
}(this, this.document));

// minified.js dependency removed
let debugMessageReconfigured = " was reconfigured.";
let websocket;
let ipEsp = "ws://192.168.4.1";
let debug = true;
let command = 1;
let hsb = [
	[0, 100, 50],
	[120, 100, 50],
	[240, 100, 50]
];
let colorPosition = 0;
let effectBri = 2;
let effectSpeed = 10;
let langVar = [0, 0, 0, 0, 0];
let layVar = [0, 0, 0, 0, 0, 0];
let itIsVar = 0;
let hasSpecialWordHappyBirthday = 0;
let autoBrightDisplay = 0;
let autoBrightEnabled = 0;
let autoBrightInterval = null;
let autoBrightMin = 10;
let autoBrightMax = 80;
let autoBrightPeak = 750;
let transitionType = 0;
let transitionDuration = 1;
let transitionSpeed = 30;
let transitionColorize = 1;
let transitionDemo = false;

const CMD = {
	// Modes
	MODE_WORD_CLOCK: 1,
	MODE_SECONDS: 2,
	MODE_SCROLLINGTEXT: 3,
	MODE_RAINBOWCYCLE: 4,
	MODE_RAINBOW: 5,
	MODE_COLOR: 6,
	MODE_DIGITAL_CLOCK: 7,
	MODE_SYMBOL: 8,
	MODE_TRANSITION: 10,
	MODE_GAME: 11,
	MODE_TIMER: 12,
	MODE_MESSAGE: 13,
	MODE_AMBIENT: 14,

	// Settings
	SET_INITIAL_VALUES: 20,
	SET_TIME: 30,
	SET_BIRTHDAYS: 83,
	SET_LANGUAGE_VARIANT: 84,
	SET_MQTT: 85,
	SET_TIME_MANUAL: 86,
	SET_BUILDTYPE: 87,
	SET_COLORTYPE: 88,
	SET_CLOCK_TYPE: 89,
	SET_WEATHER_DATA: 90,
	SET_WHITETYPE: 91,
	SET_HOSTNAME: 92,
	SET_SETTING_SECOND: 93,
	SET_MINUTE: 94,
	SET_BRIGHTNESS: 95,
	SET_SCROLLINGTEXT: 96,
	SET_TIMESERVER: 97,
	SET_WIFI_DISABLED: 98,
	SET_WIFI_AND_RESTART: 99,
	RESET: 100,
	SET_BOOT: 101,
	SET_AUTO_BRIGHT: 102,
	SET_LAYOUT_VARIANT: 103,
	SET_MQTT_HA_DISCOVERY: 104,
	SET_SYMBOL: 105,
	SET_IT_IS_VARIANT: 106,
	SET_HARDWARE_PINS: 107,
	SET_TIMEZONE: 108,
	SET_WC24H_DISPLAY_MODE: 109,
	SET_NIGHT_TIMERS: 110,
	SET_OVERLAYS: 111,
	SET_COLOR_ANIMATION: 112,
	GAME_INPUT: 113,
	SET_DIGITAL_CLOCK: 114,
	SET_ALARMS: 115,
	SET_WORD_COLORS: 116,
	SHOW_NOTIFICATION: 117,
	SPEED: 152,

	// Requests
	REQ_CONFIG_VALUES: 200,
	REQ_COLOR_VALUES: 201,
	REQ_WIFI_LIST: 202,
	REQ_AUTO_BRIGHT: 203,
	REQ_TRANSITION: 204,
	REQ_MQTT_VALUES: 205,
	REQ_BIRTHDAYS: 206,
	REQ_I2C_SCAN: 207,
	REQ_WC24H: 208,
	REQ_FRAME: 209,
	REQ_HIGHSCORES: 210
};

const NIGHT_TIMER_FLAG_ACTIVE = 0x80;
const NIGHT_TIMER_FLAG_SWITCH_ON = 0x40;
const OVERLAY_FLAG_ACTIVE = 0x01;
const OVERLAY_TEXT_LENGTH = 23;
// value: i18n key, in the order shown in the type selection
const OVERLAY_TYPES = [[0, "none"], [1, "icon"], [4, "weather-icon"], [2, "date"],
	[3, "temperature-words"], [10, "temperature-digits"], [6, "ticker"], [20, "days-until"], [21, "moon"]];
const OVERLAY_TYPE_DAYS_UNTIL = 20;
const OVERLAY_TYPE_ICON = 1;
const OVERLAY_TYPE_TICKER = 6;
const OVERLAY_DATE_FIXED = 1;
const OVERLAY_DATE_CODES = ["always", "fixed", "carnival-monday", "easter-sunday",
	"advent1", "advent2", "advent3", "advent4"];

/**
 * Maps the mode to the corresponding input id on the functions page.
 *
 * @type {Map<number, String>}
 */
const MODE_TO_INPUT_ID = new Map();
MODE_TO_INPUT_ID.set(0, "mode-wordclock"); // Map CMD.IDLE to mode wordclock
MODE_TO_INPUT_ID.set(CMD.MODE_WORD_CLOCK, "mode-wordclock");
MODE_TO_INPUT_ID.set(CMD.MODE_SECONDS, "mode-seconds");
MODE_TO_INPUT_ID.set(CMD.MODE_SCROLLINGTEXT, "mode-scrollingtext");
MODE_TO_INPUT_ID.set(CMD.MODE_RAINBOWCYCLE, "mode-rainbow");
MODE_TO_INPUT_ID.set(CMD.MODE_RAINBOW, "mode-change"); // Color change
MODE_TO_INPUT_ID.set(CMD.MODE_COLOR, "mode-color");
MODE_TO_INPUT_ID.set(CMD.MODE_DIGITAL_CLOCK, "mode-digital-clock");
MODE_TO_INPUT_ID.set(CMD.MODE_SYMBOL, "mode-symbol");
MODE_TO_INPUT_ID.set(CMD.MODE_TRANSITION, "mode-wordclock");

const MODE_CONTROL_STATE = {
	"mode-wordclock": { cmd: CMD.MODE_WORD_CLOCK, bri: false, speed: false, txt: false, symbol: false },
	"mode-seconds": { cmd: CMD.MODE_SECONDS, bri: false, speed: false, txt: false, symbol: false },
	"mode-scrollingtext": { cmd: CMD.MODE_SCROLLINGTEXT, bri: false, speed: true, txt: true, symbol: false },
	"mode-rainbow": { cmd: CMD.MODE_RAINBOWCYCLE, bri: true, speed: true, txt: false, symbol: false },
	"mode-change": { cmd: CMD.MODE_RAINBOW, bri: true, speed: true, txt: false, symbol: false },
	"mode-color": { cmd: CMD.MODE_COLOR, bri: false, speed: false, txt: false, symbol: false },
	"mode-digital-clock": { cmd: CMD.MODE_DIGITAL_CLOCK, bri: false, speed: false, txt: false, symbol: false },
	"mode-symbol": { cmd: CMD.MODE_SYMBOL, bri: false, speed: false, txt: false, symbol: true }
};

// data that gets send back to the esp
const DATA_SCROLLINGTEXT_LENGTH = 30;
const DATA_TIMESERVER_TEXT_LENGTH = 30;
const DATA_TIMEZONE_TEXT_LENGTH = 30;
const DATA_MQTT_RESPONSE_TEXT_LENGTH = 30;
const DATA_HOST_TEXT_LENGTH = 30;
const DEFAULT_TIMEZONE = "CET-1CEST,M3.5.0,M10.5.0/3";

// color pickers
let colorPicker;

/**
 * Indicates local development of the webinterface.
 * @returns {boolean|boolean}
 */
function isLocalEnvironment() {
	// location.host will be an empty string if loading the HTML from file:/// protocol.
	return location.host === "" || location.host.includes("localhost");
}
function initConfigValues() {

	// Load from hardcoded IP when doing development locally.
	if (isLocalEnvironment()) {
		ipEsp = "ws://192.168.2.167/";
	} else {
		ipEsp = `ws://${location.host}`;
	}

	debug = true;
	command = 1;
	hsb = [
		[0, 100, 50],
		[120, 100, 50],
		[240, 100, 50]
	];
	effectBri = 2;
	effectSpeed = 10;
	langVar = [0, 0, 0, 0, 0];
	layVar = [0, 0, 0, 0, 0, 0];
	itIsVar = 0;
	hasSpecialWordHappyBirthday = 0;
	autoBrightDisplay = 0;
	autoBrightEnabled = 0;
	autoBrightMin = 10;
	autoBrightMax = 80;
	autoBrightPeak = 750;
	transitionType = 0;
	transitionDuration = 1;
	transitionSpeed = 30;
	transitionColorize = 1;
	transitionDemo = false;
}

/* eslint-disable no-console */
function debugMessage(debugMessage, someObject) {
	if (debug === true) {

		if (console !== undefined) {
			if (someObject) {
				console.log(debugMessage, someObject);
			} else {
				console.log(debugMessage);
			}
		}

		const outputEl = document.getElementById("output");
		if (outputEl) outputEl.textContent = debugMessage;
	}
}

/// only shows elements of class `cls` if `enbl` is true.
function enableSpecific(cls, enbl) {
	let items = document.getElementsByClassName(cls);
	for (const item of items) {
		item.style.display = enbl ? "block" : "none";
	}
}

function updateMinuteOptions(supportedMinuteVariants) {
	const selectobject = document.getElementById("show-minutes");
	if (!selectobject || supportedMinuteVariants === undefined) return;

	for (const option of selectobject.options) {
		const supported = (supportedMinuteVariants & (1 << Number(option.value))) !== 0;
		option.disabled = !supported;
		option.hidden = !supported;
	}

	const selectedOption = selectobject.selectedOptions[0];
	if (selectedOption && selectedOption.disabled) {
		selectobject.value = "0";
	}
}

function supportsMinuteDirection(supportedMinuteVariants) {
	const led4x = 1 << 1;
	const led7x = 1 << 2;
	const corners = 1 << 3;
	return (supportedMinuteVariants & (led4x | led7x | corners)) !== 0;
}

// handle click events on the swatch

let swatchGrid = document.getElementById("swatch-grid");
if (swatchGrid) {
	swatchGrid.addEventListener("click", function(ext) {
		let clickTarget = ext.target;
		// read data-color attribute
		if (clickTarget.dataset.color) {
			// update the color picker
			colorPicker.color.set(clickTarget.dataset.color);
			changeColor(colorPicker.color);
		}
	});
}

function initWebsocket() {
	websocket = new WebSocket(ipEsp);

	websocket.onopen = function(event) {

		const statusEl = document.getElementById("status");
		if (statusEl) {
			statusEl.classList.add("online");
			statusEl.classList.remove("offline");
			statusEl.setAttribute("value", "Online");
		}

		document.querySelectorAll(".status-button").forEach(btn => {
			btn.textContent = "Disconnect";
			btn.value = "1";
		});

		const connLostEl = document.getElementById("section-connection-lost");
		if (connLostEl) connLostEl.style.display = "none";

		debugMessage("The connection with the websocket has been established.", event);

		sendCmd(CMD.REQ_COLOR_VALUES);
		sendCmd(CMD.REQ_TRANSITION);
		sendCmd(CMD.REQ_CONFIG_VALUES);
	};

	websocket.onclose = function(event) {

		const statusEl = document.getElementById("status");
		if (statusEl) {
			statusEl.classList.remove("online");
			statusEl.classList.add("offline");
			statusEl.setAttribute("value", "Offline");
		}

		document.querySelectorAll(".status-button").forEach(btn => {
			btn.textContent = "Verbinden";
			btn.value = "0";
		});

		const connLostEl = document.getElementById("section-connection-lost");
		if (connLostEl) connLostEl.style.display = "block";

		autoBrightStop();

		debugMessage(`The connection with the websocket was closed (code ${event.code}).`, event);
	};

	websocket.onmessage = function(event) {

		const modeColorForm = document.getElementById("mode-color-form");

		let data = JSON.parse(event.data);

		debugMessage(`WebSocket response arrived (command ${data.command}).`, data);

		switch (data.command) {
			case "game": {
				renderGameStatus(data);
				break;
			}
			case "timer": {
				renderTimerStatus(data);
				break;
			}
			case "frame": {
				frameWaiting = false;
				renderFrame(data.rgb);
				renderStatusLine(data);
				break;
			}
			case "highscores": {
				renderHighscores(data);
				break;
			}
			case "alarm": {
				document.getElementById("alarm-ringing").hidden = !data.ringing;
				if (data.ringing) vibrate([300, 200, 300]);
				break;
			}
			case "mqtt": {
				document.getElementById("mqtt-port").value = data.MQTT_Port;
				const mqttServer = document.getElementById("mqtt-server");
				mqttServer.value = data.MQTT_Server;
				mqttServer.setAttribute("maxlength", DATA_MQTT_RESPONSE_TEXT_LENGTH);
				document.getElementById("mqtt-state").checked = data.MQTT_State;
				const mqttUser = document.getElementById("mqtt-user");
				mqttUser.value = data.MQTT_User;
				mqttUser.setAttribute("maxlength", DATA_MQTT_RESPONSE_TEXT_LENGTH);
				const mqttPass = document.getElementById("mqtt-pass");
				mqttPass.value = data.MQTT_Pass;
				mqttPass.setAttribute("maxlength", DATA_MQTT_RESPONSE_TEXT_LENGTH);
				const mqttClientId = document.getElementById("mqtt-clientid");
				mqttClientId.value = data.MQTT_ClientId;
				mqttClientId.setAttribute("maxlength", DATA_MQTT_RESPONSE_TEXT_LENGTH);
				const mqttTopic = document.getElementById("mqtt-topic");
				mqttTopic.value = data.MQTT_Topic;
				mqttTopic.setAttribute("maxlength", DATA_MQTT_RESPONSE_TEXT_LENGTH);
				break;
			}
			case "wc24h":
				renderWc24h(data);
				break;

			case "birthdays":
				hasSpecialWordHappyBirthday = data.hasSpecialWordHappyBirthday;
				for (let i = 0; i < 5; i++) {
					const el = document.getElementById(`birthdays-date${i}`);
					if (el) el.value = data[`birthdayDate${i}`];
				}
				break;

			case "config": {
				document.getElementById("ssid").value = data.ssid;
				document.getElementById("timeserver").value = data.timeserver;
				const timezone = document.getElementById("timezone");
				timezone.value = data.timezone || DEFAULT_TIMEZONE;
				timezone.setAttribute("maxlength", DATA_TIMEZONE_TEXT_LENGTH);
				document.getElementById("hostname").value = data.hostname;
				document.getElementById("scrollingtext").value = data.scrollingText;

				[6, 8, 12, 16, 18, 20, 22, 24].forEach(h => {
					const el = document.getElementById(`brightness-${h}`);
					if (el) el.value = data[`h${h}`];
				});

				for (let i = 0; i < 5; i++) {
					const langVarEl = document.getElementById(`langvar-${i}`);
					if (langVarEl) langVarEl.value = data[`langVar${i}`];
				}

				for (let i = 0; i < 6; i++) {
					const layvarEl = document.getElementById(`layvar-${i}`);
					if (layvarEl) layvarEl.checked = data[`layVar${i}`];
				}

				itIsVar = data.itIsVariant;
				document.getElementById("it-is-variant").value = data.itIsVariant;
				document.getElementById("slider-brightness").value = data.effectBri;
				document.getElementById("slider-speed").value = data.effectSpeed;
				document.getElementById("show-seconds").value = data.secondVariant;
				document.getElementById("show-minutes").value = data.minuteVariant;
				updateMinuteOptions(data.supportedMinuteVariants);

				document.getElementById("owm-api-key").value = data.apiKey;
				document.getElementById("owm-city-id").value = data.cityid;

				hasSpecialWordHappyBirthday = data.hasSpecialWordHappyBirthday;
				document.getElementById("front-layout").value = data.clockTypeDef;
				document.getElementById("buildtype").value = data.buildtype;
				document.getElementById("whitetype").value = data.wType;
				document.getElementById("colortype").value = data.colortype;
				setSelectedSymbol(data.bitmapSymbol);
				document.getElementById("led-pin").value = data.ledPin;
				document.getElementById("power-button-pin").value = data.powerButtonPin;
				document.getElementById("mode-button-pin").value = data.modeButtonPin;
				document.getElementById("speed-button-pin").value = data.speedButtonPin;
				document.getElementById("i2c-sda-pin").value = data.i2cSdaPin;
				document.getElementById("i2c-scl-pin").value = data.i2cSclPin;

				document.getElementById("boot-show-led-blink").checked = data.bootLedBlink;
				document.getElementById("boot-show-led-sweep").checked = data.bootLedSweep;
				document.getElementById("boot-show-wifi").checked = data.bootShowWifi;
				document.getElementById("boot-show-ip").checked = data.bootShowIP;
				if (data.bootSpeed !== undefined) {
					document.getElementById("boot-speed").value = data.bootSpeed;
					document.getElementById("boot-speed-value").textContent = data.bootSpeed;
				}
				if (data.digitalClockFlags !== undefined) {
					setDigitalClockOptions(data.digitalClockFlags);
				}
				renderExtrasConfig(data);

				if (modeColorForm) {
					modeColorForm.style.gridTemplateColumns = data.hasSecondsFrame ? "1fr 1fr 1fr" : "1fr 1fr";
				}

				enableSpecific("specific-layout-0", !data.isRomanLanguage);
				enableSpecific("specific-layout-2", data.hasDreiviertel);
				enableSpecific("specific-layout-3", data.hasTwenty && data.clockTypeDef !== 4);
				enableSpecific("specific-layout-4", data.hasSecondsFrame);
				enableSpecific("specific-layout-5", data.hasWeatherLayout);

				// clockTypeDef 4 is EN10x11, doesn't need the options of layout 1 and 3, but needs its own layout 6.
				enableSpecific("specific-layout-6", data.clockTypeDef === 4);
				enableSpecific("specific-layout-1", data.clockTypeDef !== 4);

				enableSpecific("specific-layout-7", data.hasSecondsFrame);
				enableSpecific("specific-colortype-4", data.colortype === 5);
				enableSpecific("specific-minute-direction", supportsMinuteDirection(data.supportedMinuteVariants));

				autoBrightEnabled = data.autoBrightEnabled;
				const autoBrightSelect = document.getElementById("auto-bright-enabled");
				if (autoBrightEnabled === 1) {
					if (autoBrightSelect) autoBrightSelect.value = 1;
					autoBrightUpdater();
				} else {
					if (autoBrightSelect) autoBrightSelect.value = 0;
					autoBrightStop();
				}
				enableSpecific("specific-layout-brightness-man", autoBrightEnabled !== 1);
				enableSpecific("specific-layout-brightness-auto", autoBrightEnabled === 1);
				document.getElementById("auto-bright-min").value = autoBrightMin;
				document.getElementById("auto-bright-max").value = autoBrightMax;
				document.getElementById("auto-bright-peak").value = autoBrightPeak;
				break;
			}
			case "i2c-scan": {
				const result = document.getElementById("i2c-scan-result");
				if (!result) break;

				if (data.status === "disabled") {
					result.value = i18next.t("settings.hardware-pins.i2c-disabled");
				} else if (data.status === "invalid") {
					result.value = i18next.t("settings.hardware-pins.i2c-invalid");
				} else if (data.addresses.length === 0) {
					result.value = i18next.t("settings.hardware-pins.i2c-none");
				} else {
					result.value = `${i18next.t("settings.hardware-pins.i2c-found")} ${data.addresses.join(", ")}`;
				}
				break;
			}
			case "set": {
				hsb[0] = [data.hsb00, data.hsb01, data.hsb02];
				hsb[1] = [data.hsb10, data.hsb11, data.hsb12];
				if (data.hsb20 !== undefined && data.hsb21 !== undefined && data.hsb22 !== undefined) {
					hsb[2] = [data.hsb20, data.hsb21, data.hsb22];
				}
				effectBri = data.effectBri;
				effectSpeed = data.effectSpeed;
				hasSpecialWordHappyBirthday = data.hasSpecialWordHappyBirthday;
				setSelectedSymbol(data.bitmapSymbol);

				if (modeColorForm) {
					modeColorForm.style.gridTemplateColumns = data.hasSecondsFrame ? "1fr 1fr 1fr" : "1fr 1fr";
				}

				command = data.prog === 0 ? CMD.MODE_WORD_CLOCK : data.prog;
				const inputID = MODE_TO_INPUT_ID.get(data.prog);
				debugMessage(`Mode is ${data.prog} (${inputID})`);

				const inputEl = document.getElementById(inputID);
				if (inputEl) inputEl.checked = true;

				setSliders();
				setColors();
				enableSpecific("specific-colortype-4", data.colortype === 5);
				enableSpecific("specific-layout-7", data.hasSecondsFrame);
				setElementsForFunctionsMenu();
				break;
			}
			case "wlan":
				document.getElementById("wlanlist").innerHTML = data.list;
				break;

			case "transition":
				transitionType = data.transitionType;
				transitionDuration = data.transitionDuration;
				transitionSpeed = data.transitionSpeed;
				transitionColorize = data.transitionColorize;
				transitionDemo = data.transitionDemo;
				setElementsForFunctionsMenu();
				break;

			case "autoBright":
				document.getElementById("auto-bright-enabled").value = data.autoBrightEnabled;
				document.getElementById("auto-bright-sensor").value = data.autoBrightSensor;
				document.getElementById("auto-bright-source").value = data.autoBrightSource || "";
				document.getElementById("auto-bright-gain").value = data.autoBrightGain;
				if (data.autoBrightMin && data.autoBrightMax && data.autoBrightPeak) {
					autoBrightMin = data.autoBrightMin;
					autoBrightMax = data.autoBrightMax;
					autoBrightPeak = data.autoBrightPeak;
					document.getElementById("auto-bright-min").value = autoBrightMin;
					document.getElementById("auto-bright-max").value = autoBrightMax;
					document.getElementById("auto-bright-peak").value = autoBrightPeak;
				}

				if (data.autoBrightEnabled === 1) {
					autoBrightUpdater();
				} else {
					autoBrightStop();
				}
				break;
		}
	};
	websocket.onerror = function(event) {
		debugMessage("An error occurred while connecting to the websocket.", event);
	};
}

function changeColor(color) {
	hsb[colorPosition][0] = color.hue;
	hsb[colorPosition][1] = color.saturation;
	if (color.value !== 100) {
		hsb[colorPosition][2] = color.value;
	}

	setColors();
	sendColorData(command, nstr(1));
}

function createColorPicker() {
	colorPicker = new iro.ColorPicker("#color-picker", {
		wheelLightness: false,
		borderWidth: 3,
		borderColor: "#777",
		layout: [{
			component: iro.ui.Wheel
		}, {
			component: iro.ui.Slider,
			options: { sliderType: "value" }
		}, {
			component: iro.ui.Slider,
			options: { sliderType: "kelvin" }
		}]
	});
	colorPicker.on("input:change", changeColor);
}

function setColors() {
	let hsbFg = {
		h: hsb[colorPosition][0],
		s: hsb[colorPosition][1],
		v: hsb[colorPosition][2]
	};
	colorPicker.setColors([hsbFg]);
}

function setSliders() {
	const briSlider = document.getElementById("slider-brightness");
	if (briSlider) briSlider.value = effectBri;
	const speedSlider = document.getElementById("slider-speed");
	if (speedSlider) speedSlider.value = effectSpeed;

	const briValue = document.getElementById("slider-brightness-value");
	if (briValue) briValue.textContent = effectBri;
	const speedValue = document.getElementById("slider-speed-value");
	if (speedValue) speedValue.textContent = effectSpeed;
}

function setSelectedSymbol(symbolValue) {
	const symbolEl = document.querySelector(`input[name='symbol-choice'][value='${symbolValue}']`);
	if (symbolEl) symbolEl.checked = true;
}

function getSelectedModeControlState() {
	const selectedMode = document.querySelector("input[name='mode']:checked");
	return selectedMode ? MODE_CONTROL_STATE[selectedMode.id] : MODE_CONTROL_STATE["mode-wordclock"];
}

function setModeSpecificControls(selected) {
	document.querySelectorAll(".brightness").forEach(el => { el.style.display = selected.bri ? "block" : "none"; });
	document.querySelectorAll(".speed").forEach(el => { el.style.display = selected.speed ? "block" : "none"; });
	document.querySelectorAll(".functions-settings").forEach(el => { el.style.display = (selected.bri || selected.speed) ? "block" : "none"; });
	document.querySelectorAll(".text").forEach(el => { el.style.display = selected.txt ? "block" : "none"; });
	document.querySelectorAll(".symbol").forEach(el => { el.style.display = selected.symbol ? "block" : "none"; });
}

function setElementsForFunctionsMenu() {
	const modeWordclock = document.getElementById("mode-wordclock");
	const isWordclockChecked = modeWordclock && modeWordclock.checked;
	setModeSpecificControls(getSelectedModeControlState());

	const transitionBox = document.getElementById("transition-box");
	if (transitionBox) {
		transitionBox.style.display = isWordclockChecked ? "block" : "none";
	}

	const functionsBirthdays = document.getElementById("functions-birthdays");
	if (functionsBirthdays) {
		functionsBirthdays.style.display = (hasSpecialWordHappyBirthday === true && isWordclockChecked) ? "block" : "none";
	}

	const transitionTypes = document.getElementById("transition-types");
	if (transitionTypes) transitionTypes.value = transitionType;

	const transitionDurationEl = document.getElementById("transition-duration");
	if (transitionDurationEl) transitionDurationEl.value = transitionDuration;

	const transitionSpeedValue = document.getElementById("transition-speed-value");
	if (transitionSpeedValue) transitionSpeedValue.textContent = transitionSpeed;

	const transitionSpeedEl = document.getElementById("transition-speed");
	if (transitionSpeedEl) transitionSpeedEl.value = transitionSpeed;

	const transitionColorizeEl = document.getElementById("transition-colorize");
	if (transitionColorizeEl) transitionColorizeEl.value = transitionColorize;

	const transitionDemoEl = document.getElementById("transition-demo");
	if (transitionDemoEl) transitionDemoEl.checked = transitionDemo;
}

function updateManualTimeInput() {
	const currentDate = new Date();
	const hours = currentDate.getHours().toString().padStart(2, "0");
	const minutes = currentDate.getMinutes().toString().padStart(2, "0");
	const timeEl = document.getElementById("time");
	if (timeEl) timeEl.value = `${hours}:${minutes}`;
}

function autoBrightUpdater() {
	if (autoBrightInterval !== null || (autoBrightEnabled !== 1 && autoBrightEnabled !== "1")) {
		return;
	}
	autoBrightInterval = setInterval(function() {
		const autoBrightEl = document.getElementById("auto-bright-enabled");
		if (autoBrightEl && autoBrightEl.value === "1") {
			sendCmd(CMD.REQ_AUTO_BRIGHT, 1);
		}
	}, 1000);
	debugMessage(`Start timer autoBrightInterval with ID ${autoBrightInterval}`);
}

function autoBrightStop() {
	if (autoBrightInterval === null) {
		return;
	}
	debugMessage(`Stop timer autoBrightInterval with ID ${autoBrightInterval}`);
	clearInterval(autoBrightInterval);
	autoBrightInterval = null;
}

function nstr5(number) {
	return Math.round(number).toString().padStart(5, "0");
}

function nstr(number) {
	return Math.round(number).toString().padStart(3, "0");
}

function getPaddedString(string, maxStringLength) {
	return string.padEnd(maxStringLength, " ");
}

function sendCmd(command, addData = "") {
	let data = nstr(command) + addData;
	debugMessage(`Send data: '${data}'`);
	if (websocket && websocket.readyState === WebSocket.OPEN) {
		websocket.send(data);
	}
}

function sendBrightnessData() {
	let payload = "";
	[6, 8, 12, 16, 18, 20, 22, 24].forEach(h => {
		let val = document.getElementById(`brightness-${h}`).value;
		window[`h${h}`] = val;
		payload += nstr(val);
	});
	sendCmd(CMD.SET_BRIGHTNESS, payload + nstr(effectBri));
	debugMessage(`Brightness${debugMessageReconfigured}`);
}

function sendColorData(command, addData = "") {
	sendCmd(command, nstr(colorPosition) +
	nstr(hsb[colorPosition][0]) +
	nstr(hsb[colorPosition][1]) +
	nstr(hsb[colorPosition][2]) +
	nstr(effectBri) +
	nstr(effectSpeed));
}

function renderWc24h(data) {
	const box = document.getElementById("wc24h-box");
	if (!box) {
		return;
	}
	box.style.display = data.hasWc24hTables ? "block" : "none";
	if (!data.hasWc24hTables) {
		return;
	}

	const select = document.getElementById("wc24h-display-mode");
	select.querySelectorAll("option:not([value='255'])").forEach(option => option.remove());
	data.displayModes.forEach((name, idx) => {
		const option = document.createElement("option");
		option.value = idx;
		option.textContent = name;
		select.appendChild(option);
	});
	select.value = data.displayMode;

	const container = document.getElementById("wc24h-night-timers");
	container.textContent = "";
	data.nightTimers.forEach((timer, idx) => {
		container.appendChild(createNightTimerRow(idx, timer[0], timer[1], timer[2]));
	});

	document.getElementById("wc24h-color-animation").value = data.colorAnimation;
	document.getElementById("wc24h-temperature").value = data.temperature === undefined
		? i18next.t("view.wc24h.temperature-none")
		: `${data.temperature.toFixed(1)} °C`;

	const overlays = document.getElementById("wc24h-overlays");
	overlays.textContent = "";
	data.overlays.forEach((overlay, idx) => {
		overlays.appendChild(createOverlayRow(idx, overlay, data.icons));
	});
}

// A number input with its unit around it, e.g. "every [15] min".
function createNumberInput(id, label, value, min, max, prefix, suffix) {
	const field = document.createElement("span");
	field.className = "wc24h-field";
	const input = document.createElement("input");
	input.type = "number";
	input.id = id;
	input.min = min;
	input.max = max;
	input.value = value;
	input.setAttribute("aria-label", label);
	input.title = label;
	if (prefix) {
		field.appendChild(document.createTextNode(`${prefix} `));
	}
	field.appendChild(input);
	if (suffix) {
		field.appendChild(document.createTextNode(` ${suffix}`));
	}
	return field;
}

function createOverlayRow(idx, overlay, iconNames) {
	const [type, flags, interval, duration, dateCode, month, day, days, icon, text] = overlay;
	const row = document.createElement("div");
	row.className = "wc24h-overlay";

	const active = document.createElement("input");
	active.type = "checkbox";
	active.id = `wc24h-overlay-active-${idx}`;
	active.checked = (flags & OVERLAY_FLAG_ACTIVE) !== 0;
	active.setAttribute("aria-label", i18next.t("view.wc24h.active"));
	row.appendChild(active);

	const typeSelect = document.createElement("select");
	typeSelect.id = `wc24h-overlay-type-${idx}`;
	typeSelect.setAttribute("aria-label", i18next.t("view.wc24h.overlay-type"));
	OVERLAY_TYPES.forEach(([value, key]) => {
		const option = document.createElement("option");
		option.value = value;
		option.textContent = i18next.t(`view.wc24h.overlay-type-${key}`);
		typeSelect.appendChild(option);
	});
	typeSelect.value = type;
	row.appendChild(typeSelect);

	row.appendChild(createSelect(`wc24h-overlay-icon-${idx}`, i18next.t("view.wc24h.overlay-icon"),
		iconNames, icon));

	const textInput = document.createElement("input");
	textInput.type = "text";
	textInput.id = `wc24h-overlay-text-${idx}`;
	textInput.maxLength = OVERLAY_TEXT_LENGTH;
	textInput.value = text;
	textInput.setAttribute("aria-label", i18next.t("view.wc24h.overlay-text"));
	textInput.placeholder = i18next.t("view.wc24h.overlay-text");
	row.appendChild(textInput);

	row.appendChild(createNumberInput(`wc24h-overlay-interval-${idx}`,
		i18next.t("view.wc24h.overlay-interval"), interval || 15, 1, 60,
		i18next.t("view.wc24h.overlay-interval-prefix"), i18next.t("view.wc24h.overlay-interval-suffix")));
	row.appendChild(createNumberInput(`wc24h-overlay-duration-${idx}`,
		i18next.t("view.wc24h.overlay-duration"), duration || 10, 5, 255,
		"", i18next.t("view.wc24h.overlay-duration-suffix")));

	row.appendChild(createSelect(`wc24h-overlay-date-${idx}`, i18next.t("view.wc24h.overlay-date"),
		OVERLAY_DATE_CODES.map(key => i18next.t(`view.wc24h.overlay-date-${key}`)), dateCode));

	const start = document.createElement("input");
	start.type = "text";
	start.id = `wc24h-overlay-start-${idx}`;
	start.maxLength = 5;
	start.pattern = "\\d{1,2}\\.\\d{1,2}";
	start.placeholder = i18next.t("view.wc24h.overlay-start");
	start.setAttribute("aria-label", i18next.t("view.wc24h.overlay-start"));
	start.value = month ? `${String(day).padStart(2, "0")}.${String(month).padStart(2, "0")}` : "";
	row.appendChild(start);

	row.appendChild(createNumberInput(`wc24h-overlay-days-${idx}`,
		i18next.t("view.wc24h.overlay-days"), days || 1, 1, 255,
		"", i18next.t("view.wc24h.overlay-days-suffix")));

	const update = () => updateOverlayRow(row, idx);
	typeSelect.addEventListener("change", update);
	row.querySelector(`#wc24h-overlay-date-${idx}`).addEventListener("change", update);
	update();
	return row;
}

// Shows only the inputs that matter for the overlay's type and date.
function updateOverlayRow(row, idx) {
	const element = id => row.querySelector(`#wc24h-overlay-${id}-${idx}`);
	const type = Number(element("type").value);
	const dateCode = Number(element("date").value);
	const show = (id, visible) => {
		const input = element(id);
		const field = input.parentElement.classList.contains("wc24h-field") ? input.parentElement : input;
		field.style.display = visible ? "" : "none";
	};
	const daysUntil = type === OVERLAY_TYPE_DAYS_UNTIL;
	show("icon", type === OVERLAY_TYPE_ICON);
	show("text", type === OVERLAY_TYPE_TICKER || daysUntil);
	// "days until" uses the date input for its target date and is shown every day
	show("date", !daysUntil);
	show("start", dateCode === OVERLAY_DATE_FIXED || daysUntil);
	show("days", dateCode !== 0 && !daysUntil);
	element("start").placeholder = i18next.t(daysUntil ? "view.wc24h.overlay-target" : "view.wc24h.overlay-start");
}

function toOverlayText(text) {
	const umlauts = { "ä": "ae", "ö": "oe", "ü": "ue", "Ä": "AE", "Ö": "OE", "Ü": "UE", "ß": "ss" };
	return text.replace(/[äöüÄÖÜß]/g, umlaut => umlauts[umlaut])
		.replace(/[^\x20-\x7E]/g, "")
		.substring(0, OVERLAY_TEXT_LENGTH);
}

function sendOverlays() {
	let payload = "";
	for (let i = 0; i < 8; i++) {
		const active = document.getElementById(`wc24h-overlay-active-${i}`);
		if (!active) {
			return;
		}
		const value = id => Number(document.getElementById(`wc24h-overlay-${id}-${i}`).value) || 0;
		const clamp = (number, min, max) => Math.min(Math.max(number, min), max);
		const match = /^(\d{1,2})\.(\d{1,2})$/.exec(document.getElementById(`wc24h-overlay-start-${i}`).value.trim());
		const day = match ? clamp(Number(match[1]), 1, 31) : 0;
		const month = match ? clamp(Number(match[2]), 1, 12) : 0;
		const text = toOverlayText(document.getElementById(`wc24h-overlay-text-${i}`).value);

		payload += nstr(value("type")) +
			nstr(active.checked ? OVERLAY_FLAG_ACTIVE : 0) +
			nstr(clamp(value("interval"), 1, 60)) +
			nstr(clamp(value("duration"), 5, 255)) +
			nstr(value("type") === OVERLAY_TYPE_DAYS_UNTIL ? 0 : value("date")) +
			nstr(month) +
			nstr(day) +
			nstr(clamp(value("days"), 1, 255)) +
			nstr(value("icon")) +
			getPaddedString(text, OVERLAY_TEXT_LENGTH);
	}
	sendCmd(CMD.SET_OVERLAYS, payload);
	debugMessage(`overlays${debugMessageReconfigured}`);
}

function createSelect(id, label, options, value) {
	const select = document.createElement("select");
	select.id = id;
	select.setAttribute("aria-label", label);
	options.forEach((text, idx) => {
		const option = document.createElement("option");
		option.value = idx;
		option.textContent = text;
		select.appendChild(option);
	});
	select.value = value;
	return select;
}

function createNightTimerRow(idx, flags, hour, minute) {
	const row = document.createElement("div");
	row.className = "wc24h-night-timer";

	const active = document.createElement("input");
	active.type = "checkbox";
	active.id = `wc24h-night-timer-active-${idx}`;
	active.checked = (flags & NIGHT_TIMER_FLAG_ACTIVE) !== 0;
	active.setAttribute("aria-label", i18next.t("view.wc24h.active"));
	row.appendChild(active);

	const weekdays = [0, 1, 2, 3, 4, 5, 6].map(day => i18next.t(`view.wc24h.weekday-${day}`));
	row.appendChild(createSelect(`wc24h-night-timer-from-${idx}`, i18next.t("view.wc24h.from-day"),
		weekdays, (flags >> 3) & 0x07));
	row.appendChild(createSelect(`wc24h-night-timer-to-${idx}`, i18next.t("view.wc24h.to-day"),
		weekdays, flags & 0x07));

	const time = document.createElement("input");
	time.type = "time";
	time.id = `wc24h-night-timer-time-${idx}`;
	time.value = `${String(hour).padStart(2, "0")}:${String(minute).padStart(2, "0")}`;
	time.setAttribute("aria-label", i18next.t("view.wc24h.time"));
	row.appendChild(time);

	row.appendChild(createSelect(`wc24h-night-timer-action-${idx}`, i18next.t("view.wc24h.action"),
		[i18next.t("view.wc24h.switch-off"), i18next.t("view.wc24h.switch-on")],
		(flags & NIGHT_TIMER_FLAG_SWITCH_ON) ? 1 : 0));

	return row;
}

function sendNightTimers() {
	let payload = "";
	for (let i = 0; i < 8; i++) {
		const active = document.getElementById(`wc24h-night-timer-active-${i}`);
		if (!active) {
			return;
		}
		const fromDay = Number(document.getElementById(`wc24h-night-timer-from-${i}`).value);
		const toDay = Number(document.getElementById(`wc24h-night-timer-to-${i}`).value);
		const switchOn = document.getElementById(`wc24h-night-timer-action-${i}`).value === "1";
		const time = document.getElementById(`wc24h-night-timer-time-${i}`).value || "00:00";
		const [hour, minute] = time.split(":");

		let flags = (fromDay << 3) | toDay;
		if (active.checked) {
			flags |= NIGHT_TIMER_FLAG_ACTIVE;
		}
		if (switchOn) {
			flags |= NIGHT_TIMER_FLAG_SWITCH_ON;
		}
		payload += nstr(flags) + nstr(hour) + nstr(minute);
	}
	sendCmd(CMD.SET_NIGHT_TIMERS, payload);
	debugMessage(`nightTimers${debugMessageReconfigured}`);
}

function showRebootRecommendedBanner() {
	const rebootRecommendedEl = document.getElementById("section-reboot-recommended");
	if (rebootRecommendedEl) rebootRecommendedEl.style.display = "block";
}

function hideRebootRecommendedBanner() {
	const rebootRecommendedEl = document.getElementById("section-reboot-recommended");
	if (rebootRecommendedEl) rebootRecommendedEl.style.display = "none";
}

// Replaced $.ready with DOMContentLoaded
document.addEventListener("DOMContentLoaded", function() {

	initConfigValues();
	createColorPicker();
	setSliders();
	setElementsForFunctionsMenu();
	initWebsocket();
	setColors();

	document.querySelectorAll("input[name='colorwheel']").forEach(input => {
		input.addEventListener("change", function(event) {
			let id = event.target.id;
			if (id === "colorwheel-frame") {
				colorPosition = 2;
			} else if (id === "colorwheel-background") {
				colorPosition = 1;
			} else {
				colorPosition = 0;
			}
			console.log(`colorPosition: ${colorPosition}`);
			setColors();
		});
	});

	document.querySelectorAll(".status-button").forEach(btn => {
		btn.addEventListener("click", function() {
			let value = this.value;
			const statusEl = document.getElementById("status");
			if (statusEl) statusEl.textContent = "Verbinden ...";

			if (value === "1") {
				value = 0;
				document.querySelectorAll(".status-button").forEach(btn => { btn.value = value; });
				if (websocket) websocket.close();
			} else {
				value = 1;
				document.querySelectorAll(".status-button").forEach(btn => { btn.value = value; });
				initWebsocket();
			}
		});
	});

	const clockBtn = document.getElementById("_clock");
	if (clockBtn) {
		clockBtn.addEventListener("click", function() {
			let date = new Date();
			let timeZoneOffset = date.getTimezoneOffset();
			timeZoneOffset = timeZoneOffset / 60 * -1;
			let time = date.getTime() / 1000;

			sendCmd(CMD.SET_TIME, getPaddedString(nstr(timeZoneOffset) + time, 21));
			debugMessage("Clock data: ");
		});
	}

	const menuLinks = document.querySelectorAll(".pure-menu-link[data-navigation]");

	function setActiveNavigation(navigation) {
		menuLinks.forEach(elem => {
			const isActive = elem.dataset.navigation === navigation;
			elem.classList.toggle("pure-menu-selected", isActive);
			elem.toggleAttribute("aria-current", isActive);
			if (isActive) elem.setAttribute("aria-current", "page");
		});
	}

	/**
	 * A menu item has been clicked.
	 */
	document.querySelectorAll("[data-navigation]").forEach(navLink => {
		navLink.addEventListener("click", function(event) {
			event.preventDefault();
			let navigation = this.dataset.navigation;

			setActiveNavigation(navigation);

			if (navigation === "functions") {
				sendCmd(CMD.REQ_BIRTHDAYS);
				setElementsForFunctionsMenu();
			}
			if (navigation === "smart-home") {
				sendCmd(CMD.REQ_MQTT_VALUES);
			}
			if (navigation === "frontoptions") {
				sendCmd(CMD.REQ_CONFIG_VALUES);
				sendCmd(CMD.REQ_WC24H);
			}
			if (navigation === "extras") {
				sendCmd(CMD.REQ_CONFIG_VALUES);
			}
			if (navigation === "home" || navigation === "games") {
				requestFrame();
			}
			document.querySelectorAll(".bottom-nav [data-navigation]").forEach(btn => {
				btn.classList.toggle("active", btn.dataset.navigation === navigation);
			});
			document.body.classList.remove("menu-open");
			if (navigation === "main" || navigation === "settings" || navigation === "frontoptions") {
				sendCmd(CMD.REQ_CONFIG_VALUES);
				sendCmd(CMD.REQ_AUTO_BRIGHT);
				updateManualTimeInput();
				autoBrightUpdater();
			} else {
				autoBrightStop();
			}

			// show/hide sections
			document.querySelectorAll(".section").forEach(sec => { sec.style.display = "none"; });
			const targetSection = document.querySelector(`.section-${navigation}`);
			if (targetSection) targetSection.style.display = "block";
		});
	});

	setActiveNavigation("home");

	/**
	 * The clock mode has been changed.
	 */
	document.querySelectorAll("input[name='mode']").forEach(input => {
		input.addEventListener("change", function(event) {
			let id = event.target.id;
			let selected = MODE_CONTROL_STATE[id];
			if (selected) {
				command = selected.cmd;
				setElementsForFunctionsMenu();

				sendColorData(command);
				setSliders();
				setColors();
			}
		});
	});

	let sliderTimeout = null;

	document.querySelectorAll("[id*='slider']").forEach(slider => {
		slider.addEventListener("input", function(event) {
			if (sliderTimeout) return;

			let id = event.target.id;
			if (id === "slider-brightness") {
				effectBri = event.target.value;
				sendBrightnessData();
			}
			if (id === "slider-speed") {
				effectSpeed = event.target.value;
				sendCmd(CMD.SPEED, nstr(effectSpeed));
			}
			setSliders();

			sliderTimeout = setTimeout(function() {
				sliderTimeout = null;
			}, 20);
		});
	});

	const transSpeedEl = document.getElementById("transition-speed");
	if (transSpeedEl) {
		transSpeedEl.addEventListener("input", function(event) {
			transitionSpeed = event.target.value;
			const tsv = document.getElementById("transition-speed-value");
			if (tsv) tsv.textContent = transitionSpeed;
		});
	}

	document.querySelectorAll("[id*='transition']").forEach(el => {
		el.addEventListener("change", function(event) {
			transitionType = document.getElementById("transition-types").value;
			transitionDuration = document.getElementById("transition-duration").value;
			transitionSpeed = document.getElementById("transition-speed").value;
			transitionColorize = document.getElementById("transition-colorize").value;
			transitionDemo = document.getElementById("transition-demo").checked;

			sendCmd(CMD.MODE_TRANSITION, nstr(transitionType) + nstr(transitionDuration) + nstr(transitionSpeed) + nstr(transitionColorize) + nstr(transitionDemo ? 1 : 0));
			debugMessage(`Transition${debugMessageReconfigured}`);
			setElementsForFunctionsMenu();
		});
	});

	const initValsBtn = document.getElementById("initial-values-button");
	if (initValsBtn) {
		initValsBtn.addEventListener("click", () => sendColorData(CMD.SET_INITIAL_VALUES));
	}

	const wifiBtn = document.getElementById("wifi-button");
	if (wifiBtn) {
		wifiBtn.addEventListener("click", function() {
			sendCmd(CMD.SET_WIFI_AND_RESTART);
			debugMessage(`WiFi${debugMessageReconfigured}`);
		});
	}

	document.querySelectorAll("[id*='auto-bright']").forEach(el => {
		el.addEventListener("change", function(event) {
			autoBrightEnabled = document.getElementById("auto-bright-enabled").value;
			autoBrightMin = document.getElementById("auto-bright-min").value;
			autoBrightMax = document.getElementById("auto-bright-max").value;

			if (parseInt(autoBrightMin) > parseInt(autoBrightMax)) {
				autoBrightMax = autoBrightMin;
				document.getElementById("auto-bright-max").value = autoBrightMax;
			}

			autoBrightPeak = document.getElementById("auto-bright-peak").value;
			sendCmd(CMD.SET_AUTO_BRIGHT, nstr(autoBrightEnabled) + nstr(autoBrightMin) + nstr(autoBrightMax) + nstr(autoBrightPeak));
			sendCmd(CMD.REQ_AUTO_BRIGHT);

			autoBrightDisplay = Number(document.getElementById("auto-bright-enabled").value);
			enableSpecific("specific-layout-brightness-man", !autoBrightDisplay);
			enableSpecific("specific-layout-brightness-auto", autoBrightDisplay);
		});
	});

	const wlanScanBtn = document.getElementById("_wlanscan");
	if (wlanScanBtn) {
		wlanScanBtn.addEventListener("click", function() {
			sendCmd(CMD.REQ_WIFI_LIST);
			document.getElementById("wlanlist").innerHTML = "<div>WLAN Netzwerke werden gesucht</div>";
		});
	}

	const timeServerBtn = document.getElementById("timeserver-button");
	if (timeServerBtn) {
		timeServerBtn.addEventListener("click", function() {
			sendCmd(CMD.SET_TIMESERVER, getPaddedString(document.getElementById("timeserver").value, DATA_TIMESERVER_TEXT_LENGTH));
			debugMessage(`Timeserver${debugMessageReconfigured}`);
		});
	}

	const timezoneBtn = document.getElementById("timezone-button");
	if (timezoneBtn) {
		timezoneBtn.addEventListener("click", function() {
			sendCmd(CMD.SET_TIMEZONE, getPaddedString(document.getElementById("timezone").value, DATA_TIMEZONE_TEXT_LENGTH));
			sendCmd(CMD.REQ_CONFIG_VALUES);
			debugMessage(`Timezone${debugMessageReconfigured}`);
		});
	}

	const scrollTextBtn = document.getElementById("scrollingtext-button");
	if (scrollTextBtn) {
		scrollTextBtn.addEventListener("click", function() {
			sendCmd(CMD.SET_SCROLLINGTEXT, getPaddedString(document.getElementById("scrollingtext").value, DATA_SCROLLINGTEXT_LENGTH));
			debugMessage(`ScrollingText${debugMessageReconfigured}`);
		});
	}

	const functionsSettingsForm = document.getElementById("functions-settings-form");
	if (functionsSettingsForm) {
		functionsSettingsForm.addEventListener("submit", function(event) {
			event.preventDefault();
		});
	}

	document.querySelectorAll("[id*='brightness']").forEach(el => {
		el.addEventListener("change", () => sendBrightnessData(CMD.SET_BRIGHTNESS));
	});

	const weatherBtn = document.getElementById("weather-button");
	if (weatherBtn) {
		weatherBtn.addEventListener("click", function() {
			sendCmd(CMD.SET_WEATHER_DATA, document.getElementById("owm-city-id").value + " " + document.getElementById("owm-api-key").value);
			debugMessage(`OpenWeatherMap Login${debugMessageReconfigured}`);
		});
	}

	const showMinsBtn = document.getElementById("show-minutes");
	if (showMinsBtn) {
		showMinsBtn.addEventListener("change", function() {
			sendCmd(CMD.SET_MINUTE, nstr(this.value));
			debugMessage(`MinuteVariant${debugMessageReconfigured}`);
		});
	}

	const showSecsBtn = document.getElementById("show-seconds");
	if (showSecsBtn) {
		showSecsBtn.addEventListener("change", function() {
			sendCmd(CMD.SET_SETTING_SECOND, nstr(this.value));
			debugMessage(`SecondVariant${debugMessageReconfigured}`);
		});
	}

	const frontLayoutBtn = document.getElementById("front-layout");
	if (frontLayoutBtn) {
		frontLayoutBtn.addEventListener("change", function() {
			sendCmd(CMD.SET_CLOCK_TYPE, nstr(this.value));
			sendCmd(CMD.REQ_CONFIG_VALUES);
			debugMessage(`FrontLayout${debugMessageReconfigured}`);
		});
	}

	const colorTypeBtn = document.getElementById("colortype-button");
	if (colorTypeBtn) {
		colorTypeBtn.addEventListener("click", function() {
			sendCmd(CMD.SET_COLORTYPE, nstr(document.getElementById("colortype").value));
			debugMessage(`Colortype${debugMessageReconfigured}`);
		});
	}

	const hardwarePinsBtn = document.getElementById("hardware-pins-button");
	if (hardwarePinsBtn) {
		hardwarePinsBtn.addEventListener("click", function() {
			const pinInputs = [
				document.getElementById("led-pin"),
				document.getElementById("power-button-pin"),
				document.getElementById("mode-button-pin"),
				document.getElementById("speed-button-pin"),
				document.getElementById("i2c-sda-pin"),
				document.getElementById("i2c-scl-pin")
			];

			pinInputs.forEach(input => {
				input.setCustomValidity("");
			});
			if (pinInputs.some(input => !input.reportValidity())) {
				return;
			}

			const pins = pinInputs.map(input => input.value);
			const i2cPins = pins.slice(4);
			const hasIncompleteI2cPins = i2cPins.includes("255") && !i2cPins.every(pin => pin === "255");
			if (hasIncompleteI2cPins) {
				pinInputs[4].setCustomValidity(i18next.t("settings.hardware-pins.i2c-pair"));
				pinInputs[4].reportValidity();
				return;
			}

			const assignedPins = pins.filter(pin => pin !== "255");
			const hasDuplicate = new Set(assignedPins).size !== assignedPins.length;
			if (hasDuplicate) {
				pinInputs.forEach(input => {
					input.setCustomValidity(i18next.t("settings.hardware-pins.duplicate"));
				});
				pinInputs[0].reportValidity();
				return;
			}

			sendCmd(CMD.SET_HARDWARE_PINS, pins.map(nstr).join(""));
			sendCmd(CMD.REQ_CONFIG_VALUES);
			showRebootRecommendedBanner();
			debugMessage(`Hardware pins${debugMessageReconfigured}`);
		});
	}

	const i2cScanBtn = document.getElementById("i2c-scan-button");
	if (i2cScanBtn) {
		i2cScanBtn.addEventListener("click", function() {
			const i2cInputs = [
				document.getElementById("i2c-sda-pin"),
				document.getElementById("i2c-scl-pin")
			];
			const result = document.getElementById("i2c-scan-result");

			i2cInputs.forEach(input => {
				input.setCustomValidity("");
			});
			if (i2cInputs.some(input => !input.reportValidity())) {
				return;
			}

			const pins = i2cInputs.map(input => input.value);
			const hasIncompleteI2cPins = pins.includes("255") && !pins.every(pin => pin === "255");
			if (hasIncompleteI2cPins) {
				i2cInputs[0].setCustomValidity(i18next.t("settings.hardware-pins.i2c-pair"));
				i2cInputs[0].reportValidity();
				return;
			}

			if (result) {
				result.value = i18next.t("settings.hardware-pins.i2c-scanning");
			}
			sendCmd(CMD.REQ_I2C_SCAN, pins.map(nstr).join(""));
			debugMessage("I2C scan requested");
		});
	}

	document.querySelectorAll('input[name="symbol-choice"]').forEach(el => {
		el.addEventListener("change", function(event) {
			let selectedSymbol = event.target.value;
			sendCmd(CMD.SET_SYMBOL, nstr(selectedSymbol));
			debugMessage(`Symbol${debugMessageReconfigured}`);
		});
	});

	const hostNameBtn = document.getElementById("hostname-button");
	if (hostNameBtn) {
		hostNameBtn.addEventListener("click", function() {
			sendCmd(CMD.SET_HOSTNAME, getPaddedString(document.getElementById("hostname").value, DATA_HOST_TEXT_LENGTH));
			debugMessage(`Hostname${debugMessageReconfigured}`);
		});
	}

	document.querySelectorAll("[id*='boot-show'], #boot-speed").forEach(el => {
		el.addEventListener("change", function() {
			let blink = document.getElementById("boot-show-led-blink").checked ? 1 : 0;
			let sweep = document.getElementById("boot-show-led-sweep").checked ? 1 : 0;
			let wifi = document.getElementById("boot-show-wifi").checked ? 1 : 0;
			let ip = document.getElementById("boot-show-ip").checked ? 1 : 0;
			let speed = document.getElementById("boot-speed").value;

			sendCmd(CMD.SET_BOOT, nstr(blink) + nstr(sweep) + nstr(wifi) + nstr(ip) + nstr(speed));
			debugMessage(`Bootoption${debugMessageReconfigured}`);
		});
	});
	const bootSpeed = document.getElementById("boot-speed");
	bootSpeed.addEventListener("input", () => {
		document.getElementById("boot-speed-value").textContent = bootSpeed.value;
	});

	document.querySelectorAll(".digital-option").forEach(el => {
		el.addEventListener("change", () => {
			sendCmd(CMD.SET_DIGITAL_CLOCK, nstr(getDigitalClockFlags()));
			debugMessage(`Digital clock${debugMessageReconfigured}`);
		});
	});

	const disableBtn = document.getElementById("disable-button");
	if (disableBtn) disableBtn.addEventListener("click", () => sendCmd(CMD.SET_WIFI_DISABLED));

	const resetBtn = document.getElementById("reset-button");
	if (resetBtn) resetBtn.addEventListener("click", () => sendCmd(CMD.RESET));

	const rebootRecommendedBtn = document.getElementById("reboot-recommended-button");
	if (rebootRecommendedBtn) {
		rebootRecommendedBtn.addEventListener("click", function() {
			hideRebootRecommendedBanner();
			sendCmd(CMD.RESET);
		});
	}

	document.querySelectorAll(".birthdays input").forEach(input => {
		input.addEventListener("input", function(event) {
			let val = event.target.value.replace(/\D/g, "");
			if (val.length > 2) {
				val = val.substring(0, 2) + "-" + val.substring(2, 4);
			}
			event.target.value = val;
		});
	});

	const bdaysStoreBtn = document.getElementById("birthdays-store-button");
	if (bdaysStoreBtn) {
		bdaysStoreBtn.addEventListener("click", function() {
			let payload = "";
			for (let i = 0; i < 5; i++) {
				payload += getPaddedString(document.getElementById(`birthdays-date${i}`).value, 5);
			}
			sendCmd(CMD.SET_BIRTHDAYS, payload);
		});
	}

	const uhrzeitBtn = document.getElementById("uhrzeit-button");
	if (uhrzeitBtn) {
		uhrzeitBtn.addEventListener("click", function() {
			const timeEl = document.getElementById("time");
			if (!timeEl.validity.valid) return;

			const timeString = timeEl.value;
			const [hours, minutes] = timeString.split(":");

			sendCmd(CMD.SET_TIME_MANUAL, nstr(hours) + nstr(minutes));
			debugMessage(`Time manually set to ${hours}:${minutes}`);
		});
	}

	const mqttBtn = document.getElementById("mqtt-button");
	if (mqttBtn) {
		mqttBtn.addEventListener("click", function() {
			const state = document.getElementById("mqtt-state").checked ? 1 : 0;
			const port = document.getElementById("mqtt-port").value;
			const server = document.getElementById("mqtt-server").value;
			const user = document.getElementById("mqtt-user").value;
			const pass = document.getElementById("mqtt-pass").value;
			const clientId = document.getElementById("mqtt-clientid").value;
			const topic = document.getElementById("mqtt-topic").value;

			const payload = nstr(state) +
							nstr5(port) +
							getPaddedString(server, DATA_MQTT_RESPONSE_TEXT_LENGTH) +
							getPaddedString(user, DATA_MQTT_RESPONSE_TEXT_LENGTH) +
							getPaddedString(pass, DATA_MQTT_RESPONSE_TEXT_LENGTH) +
							getPaddedString(clientId, DATA_MQTT_RESPONSE_TEXT_LENGTH) +
							getPaddedString(topic, DATA_MQTT_RESPONSE_TEXT_LENGTH);

			sendCmd(CMD.SET_MQTT, payload);
			debugMessage(`MQTT config${debugMessageReconfigured}`);
		});
	}

	const mqttDiscBtn = document.getElementById("mqtt-discovery-button");
	if (mqttDiscBtn) {
		mqttDiscBtn.addEventListener("click", function() {
			sendCmd(CMD.SET_MQTT_HA_DISCOVERY);
			debugMessage(`MQTT Discovery${debugMessageReconfigured}`);
		});
	}

	document.querySelectorAll("[id*='it-is-variant']").forEach(el => {
		el.addEventListener("change", function() {
			let payload = "";
			itIsVar = document.getElementById(`it-is-variant`).value;
			payload += nstr(itIsVar);
			sendCmd(CMD.SET_IT_IS_VARIANT, payload);
			debugMessage(`itIsVar${debugMessageReconfigured}`);
		});
	});

	const wc24hDisplayMode = document.getElementById("wc24h-display-mode");
	if (wc24hDisplayMode) {
		wc24hDisplayMode.addEventListener("change", function() {
			sendCmd(CMD.SET_WC24H_DISPLAY_MODE, nstr(wc24hDisplayMode.value));
			debugMessage(`wc24hDisplayMode${debugMessageReconfigured}`);
		});
	}

	const nightTimersStoreBtn = document.getElementById("wc24h-night-timers-store-button");
	if (nightTimersStoreBtn) {
		nightTimersStoreBtn.addEventListener("click", sendNightTimers);
	}

	const overlaysStoreBtn = document.getElementById("wc24h-overlays-store-button");
	if (overlaysStoreBtn) {
		overlaysStoreBtn.addEventListener("click", sendOverlays);
	}

	const colorAnimation = document.getElementById("wc24h-color-animation");
	if (colorAnimation) {
		colorAnimation.addEventListener("change", function() {
			sendCmd(CMD.SET_COLOR_ANIMATION, nstr(colorAnimation.value));
			debugMessage(`colorAnimation${debugMessageReconfigured}`);
		});
	}

	document.querySelectorAll("[id*='langvar']").forEach(el => {
		el.addEventListener("change", function() {
			let payload = "";
			for (let i = 0; i < 5; i++) {
				langVar[i] = document.getElementById(`langvar-${i}`).value;
				payload += nstr(langVar[i]);
			}
			sendCmd(CMD.SET_LANGUAGE_VARIANT, payload);
			debugMessage(`langVar${debugMessageReconfigured}`);
		});
	});

	document.querySelectorAll("[id*='layvar']").forEach(el => {
		el.addEventListener("change", function() {
			let payload = "";
			for (let i = 0; i < 6; i++) {
				layVar[i] = document.getElementById(`layvar-${i}`).checked ? 1 : 0;
				payload += nstr(layVar[i]);
			}
			sendCmd(CMD.SET_LAYOUT_VARIANT, payload);
			debugMessage(`layVar${debugMessageReconfigured}`);
		});
	});

	document.querySelectorAll("[id*='buildtype']").forEach(el => {
		el.addEventListener("change", function(event) {
			sendCmd(CMD.SET_BUILDTYPE, nstr(event.target.value));
			showRebootRecommendedBanner();
			debugMessage(`buildtype${debugMessageReconfigured}`);
		});
	});

	document.querySelectorAll("[id*='whitetype']").forEach(el => {
		el.addEventListener("change", function(event) {
			sendCmd(CMD.SET_WHITETYPE, nstr(event.target.value));
			debugMessage(`whitetype${debugMessageReconfigured}`);
		});
	});
	initGamesAndTimer();
});

/*
 * Games, timer, start page, live preview, alarms, appearance
 */
const GAME_KEY = {
	LEFT: 1,
	RIGHT: 2,
	UP: 3,
	DOWN: 4,
	ACTION: 5,
	PAUSE: 6,
	COLOR1: 11,
	PAINT_SET: 20,
	PAINT_FILL: 21,
	PAINT_CLEAR: 22,
	ANSWER: 30,
	TAP: 31
};
const GAME_REPEAT_DELAY_MS = 220;
const GAME_REPEAT_INTERVAL_MS = 90;
const TIMER_ACTION = { COUNTDOWN: 1, STOPWATCH: 2, PAUSE: 3, STOP: 4 };
const MATRIX_ROWS = 16;
const MATRIX_COLS = 18;
const FRONT_LETTERS = [
	"ESAISTOVIERTELEINS", "DREINERSECHSIEBENE", "ELFÜNFNEUNVIERACHT",
	"NULLZWEINZWÖLFZEHN", "UNDOZWANZIGVIERZIG", "DREISSIGFÜNFZIGUHR",
	"MINUTENIVORUNDNACH", "EINDREIVIERTELHALB", "SIEBENEUNULLZWEINE",
	"FÜNFSECHSNACHTVIER", "DREINSUNDAELFEZEHN", "ZWANZIGGRADREISSIG",
	"VIERZIGZWÖLFÜNFZIG", "MINUTENUHREFRÜHVOR", "ABENDSMITTERNACHTS",
	"MORGENSWARMMITTAGS"
];
// Same order as wc24h::Palette in Arcade.h
const PAINT_COLORS = ["#000000", "#ff0000", "#ff7700", "#ffdd00", "#00dd00", "#00dddd",
	"#0055ff", "#9933ff", "#ff66cc", "#ffffff"];

/*
 * Per game: control panel, which pad buttons are shown, key labels.
 * pad: list of pad keys to show; action: label key of the middle button.
 */
const GAMES = new Map([
	[1, { key: "tetris", controls: "pad", pad: [1, 2, 3, 4, 5, 6], action: "games.drop", hint: "games.hint-tetris" }],
	[2, { key: "snake", controls: "pad", pad: [1, 2, 3, 4, 6], hint: "games.hint-snake" }],
	[3, { key: "simon", controls: "simon", hint: "games.hint-simon" }],
	[4, { key: "flappy", controls: "pad", pad: [5, 6], action: "games.flap", hint: "games.hint-flappy" }],
	[5, { key: "breakout", controls: "pad", pad: [1, 2, 5, 6], action: "games.launch", hint: "games.hint-breakout" }],
	[6, { key: "invaders", controls: "pad", pad: [1, 2, 5, 6], action: "games.fire", hint: "games.hint-invaders" }],
	[7, { key: "frogger", controls: "pad", pad: [1, 2, 3, 4, 6], repeat: false, hint: "games.hint-frogger" }],
	[8, { key: "pong", controls: "pad", pad: [3, 4], twoPlayer: true, hint: "games.hint-pong" }],
	[9, { key: "pong-cpu", controls: "pad", pad: [3, 4], hint: "games.hint-pong-cpu" }],
	[10, { key: "connect4", controls: "pad", pad: [1, 2, 5], action: "games.drop", repeat: false, twoPlayer: true, hint: "games.hint-connect4" }],
	[11, { key: "connect4-cpu", controls: "pad", pad: [1, 2, 5], action: "games.drop", repeat: false, hint: "games.hint-connect4-cpu" }],
	[12, { key: "reaction", controls: "reaction", twoPlayer: true, hint: "games.hint-reaction" }],
	[13, { key: "hangman", controls: "keyboard", hint: "games.hint-hangman" }],
	[14, { key: "clock-quiz", controls: "quiz", hint: "games.hint-clock-quiz" }],
	[15, { key: "word-find", controls: "tap", hint: "games.hint-word-find" }],
	[16, { key: "math", controls: "math", hint: "games.hint-math" }],
	[20, { key: "paint", controls: "paint", hint: "games.hint-paint" }]
]);
const HIGHSCORE_GAMES = new Set([1, 2, 3, 4, 5, 6, 7, 13, 14, 15, 16]);
const PROG_NAMES = new Map([[0, "mode.wordclock"], [1, "mode.wordclock"], [2, "functions.mode.seconds"],
	[3, "functions.mode.scrollingtext"], [4, "functions.mode.rainbow"], [5, "functions.mode.color-change"],
	[6, "functions.mode.color"], [7, "functions.mode.digital-clock"], [8, "functions.mode.symbol"],
	[10, "mode.wordclock"], [11, "nav.games"], [12, "timer.h2"], [13, "home.message"], [14, "home.mood"],
	[15, "extras.alarm.h2"]]);

let currentGame = 0;
let lastGameStatus = null;
let paintColor = 1;
let paintFill = false;
let mathInput = "";
let frameWaiting = false;
let frameRequestedAt = 0;

function storageGet(key, fallback) {
	try {
		const value = localStorage.getItem(key);
		return value === null ? fallback : value;
	} catch (error) {
		debugMessage("Local storage is not available.", error);
		return fallback;
	}
}

function storageSet(key, value) {
	try {
		localStorage.setItem(key, value);
	} catch (error) {
		// private mode: settings just are not remembered
		debugMessage("Local storage is not available.", error);
	}
}

function playerNumber() {
	const checked = document.querySelector("input[name='player-number']:checked");
	return checked ? Number(checked.value) : 1;
}

function sendGameKey(key, value = 0) {
	sendCmd(CMD.GAME_INPUT, nstr(key) + nstr(playerNumber()) + nstr5(value));
}

function vibrate(pattern) {
	if (navigator.vibrate) {
		navigator.vibrate(pattern);
	}
}

function formatTimer(seconds) {
	const mm = Math.floor(seconds / 60).toString().padStart(2, "0");
	const ss = (seconds % 60).toString().padStart(2, "0");
	return `${mm}:${ss}`;
}

function toClockText(text) {
	const umlauts = { "ä": "ae", "ö": "oe", "ü": "ue", "Ä": "AE", "Ö": "OE", "Ü": "UE", "ß": "ss" };
	return text.replace(/[äöüÄÖÜß]/g, umlaut => umlauts[umlaut]).replace(/[^\x20-\x7E]/g, "");
}

function isSectionVisible(name) {
	const section = document.querySelector(`.section-${name}`);
	return Boolean(section) && section.style.display === "block";
}

/*
 * Live preview
 */
function initPreviews() {
	document.querySelectorAll("[data-preview]").forEach(preview => {
		for (let row = 0; row < MATRIX_ROWS; row++) {
			for (let col = 0; col < MATRIX_COLS; col++) {
				const cell = document.createElement("span");
				cell.textContent = FRONT_LETTERS[row][col];
				cell.dataset.cell = row * MATRIX_COLS + col;
				preview.appendChild(cell);
			}
		}
	});
}

function renderFrame(hex) {
	if (!hex || hex.length < MATRIX_ROWS * MATRIX_COLS * 6) {
		return;
	}
	document.querySelectorAll("[data-preview]").forEach(preview => {
		if (preview.offsetParent === null) {
			return;
		}
		const cells = preview.children;
		for (let i = 0; i < cells.length; i++) {
			const red = parseInt(hex.substr(i * 6, 2), 16);
			const green = parseInt(hex.substr(i * 6 + 2, 2), 16);
			const blue = parseInt(hex.substr(i * 6 + 4, 2), 16);
			const max = Math.max(red, green, blue);
			if (max < 3) {
				cells[i].style.color = "";
				cells[i].style.textShadow = "";
				cells[i].classList.remove("lit");
				continue;
			}
			// LEDs run dimmed; show the hue at full strength
			const scale = 255 / max;
			const color = `rgb(${Math.round(red * scale)}, ${Math.round(green * scale)}, ${Math.round(blue * scale)})`;
			cells[i].style.color = color;
			cells[i].style.textShadow = `0 0 6px ${color}`;
			cells[i].classList.add("lit");
		}
	});
}

function wifiQuality(rssi) {
	if (rssi >= -60) return "status.wifi-great";
	if (rssi >= -70) return "status.wifi-good";
	if (rssi >= -80) return "status.wifi-ok";
	return "status.wifi-weak";
}

function renderStatusLine(data) {
	const mode = i18next.t(PROG_NAMES.get(data.prog) || "mode.wordclock");
	const text = i18next.t("status.line", {
		mode,
		wifi: i18next.t(wifiQuality(data.rssi)),
		power: i18next.t(data.power ? "status.on" : "status.off")
	});
	document.querySelectorAll("[data-status-line]").forEach(el => { el.textContent = text; });
}

function requestFrame() {
	const now = Date.now();
	if (frameWaiting && now - frameRequestedAt < 1500) {
		return;
	}
	frameWaiting = true;
	frameRequestedAt = now;
	sendCmd(CMD.REQ_FRAME);
}

function startFramePolling() {
	let lastSlow = 0;
	setInterval(() => {
		if (document.visibilityState !== "visible" || !websocket || websocket.readyState !== WebSocket.OPEN) {
			return;
		}
		const fast = isSectionVisible("home") || (isSectionVisible("games") && currentGame);
		const now = Date.now();
		if (fast || now - lastSlow > 5000) {
			lastSlow = now;
			requestFrame();
		}
	}, 250);
}

/*
 * Games
 */
function showControls(info) {
	document.querySelectorAll("#game-box [data-controls]").forEach(panel => {
		panel.hidden = panel.dataset.controls !== info.controls;
	});
	const gamePreview = document.querySelector(".preview-game");
	gamePreview.hidden = !(info.controls === "tap" || info.controls === "paint");
	if (info.controls === "pad") {
		document.querySelectorAll(".gamepad .pad").forEach(pad => {
			pad.hidden = !info.pad.includes(Number(pad.dataset.key));
			if (pad.dataset.key !== "6") {
				pad.dataset.repeat = info.repeat === false ? "0" : (["1", "2", "4"].includes(pad.dataset.key) ? "1" : "0");
			}
		});
		const action = document.querySelector(".gamepad .pad-action");
		action.textContent = i18next.t(info.action || "games.drop");
	}
}

function startGame(id) {
	const info = GAMES.get(id);
	if (!info) {
		return;
	}
	currentGame = id;
	lastGameStatus = null;
	mathInput = "";
	document.getElementById("math-display").textContent = "";
	document.getElementById("game-box").hidden = false;
	document.getElementById("game-title").textContent = i18next.t(`games.${info.key}`);
	document.getElementById("game-hint").textContent = i18next.t(info.hint);
	document.getElementById("game-score-line").textContent = "";
	document.getElementById("game-state").textContent = "";
	document.getElementById("game-text").textContent = "";
	document.querySelectorAll(".letter-keys button").forEach(btn => btn.classList.remove("used", "right", "wrong"));
	showControls(info);
	const name = toClockText(document.getElementById("player-name").value).substring(0, 8);
	sendCmd(CMD.MODE_GAME, nstr(id) + name);
	document.getElementById("highscores").hidden = !HIGHSCORE_GAMES.has(id);
	if (HIGHSCORE_GAMES.has(id)) {
		sendCmd(CMD.REQ_HIGHSCORES, nstr(id));
	}
	document.getElementById("game-box").scrollIntoView({ behavior: "smooth", block: "start" });
}

function renderGameStatus(data) {
	const info = GAMES.get(data.game);
	if (!info || data.game !== currentGame) {
		return;
	}
	const previous = lastGameStatus;
	lastGameStatus = data;
	const parts = [];
	if (info.twoPlayer || data.game === 9 || data.game === 11) {
		if (data.game === 10 || data.game === 11) {
			if (!data.over) {
				parts.push(i18next.t(data.turn === 1 ? "games.turn-red" : "games.turn-yellow"));
			}
		} else {
			parts.push(i18next.t("games.two-score", { s1: data.score1 || 0, s2: data.score2 || 0 }));
		}
	} else if (data.game !== 20) {
		parts.push(i18next.t("games.score-value", { score: data.score }));
	}
	if (data.lives !== undefined && data.game !== 13) {
		parts.push("♥".repeat(Math.max(0, data.lives)));
	}
	if (data.level) {
		parts.push(i18next.t("games.level", { level: data.level }));
	}
	document.getElementById("game-score-line").textContent = parts.join(" · ");

	let text = "";
	switch (data.game) {
		case 1: text = i18next.t("games.lines", { lines: data.text }); break;
		case 2: text = i18next.t("games.length", { length: data.text }); break;
		case 3: text = i18next.t(data.text === "input" ? "games.simon-input" : "games.simon-watch"); break;
		case 4: text = data.text === "tap" ? i18next.t("games.flappy-start") : ""; break;
		case 9: case 11: text = data.text === "cpu" ? i18next.t("games.against-clock") : ""; break;
		case 12:
			text = data.text2 === "foul" ? i18next.t("games.reaction-foul")
				: i18next.t(data.text === "go" ? "games.reaction-go" : "games.reaction-wait");
			break;
		case 13:
			text = `${data.text}   ♥ ${data.lives}` + (data.text2 ? `   ✗ ${data.text2.split("").join(" ")}` : "");
			updateLetterKeys(data);
			break;
		case 14:
			text = i18next.t("games.question", { progress: data.text });
			if (data.text2) {
				const right = data.text2[0] === "+";
				text += " · " + i18next.t(right ? "games.right" : "games.wrong-time", { time: data.text2.substring(1) });
			}
			break;
		case 15: text = i18next.t("games.find", { word: data.text, progress: data.text2 }); break;
		case 16: {
			text = data.text;
			const [progress, result] = data.text2.split(" ");
			text += ` (${progress})`;
			if (result && previous && previous.text2 !== data.text2) {
				const right = result[0] === "+";
				document.getElementById("math-display").textContent =
					right ? "✓" : i18next.t("games.correct-answer", { answer: result.substring(1) });
				mathInput = "";
			}
			break;
		}
		default: break;
	}
	document.getElementById("game-text").textContent = text;

	let state = "";
	if (data.over) {
		if (data.game === 13) {
			state = i18next.t(data.winner ? "games.solved" : "games.over");
		} else if (info.twoPlayer || data.game === 9 || data.game === 11) {
			state = data.winner ? i18next.t("games.winner", { player: data.winner }) : i18next.t("games.draw");
		} else {
			state = i18next.t("games.over");
		}
		if (data.rank) {
			state += " " + i18next.t("games.new-highscore", { rank: data.rank });
		}
	} else if (data.paused) {
		state = i18next.t("games.paused");
	}
	document.getElementById("game-state").textContent = state;

	if (previous) {
		if (data.over && !previous.over) {
			vibrate([120, 60, 120]);
		} else if (data.score > previous.score || data.score1 > previous.score1 || data.score2 > previous.score2) {
			vibrate(30);
		}
	}
}

function renderHighscores(data) {
	if (data.game !== currentGame) {
		return;
	}
	const list = document.getElementById("highscore-list");
	list.innerHTML = "";
	if (!data.list.length) {
		const item = document.createElement("li");
		item.textContent = i18next.t("games.no-highscores");
		list.appendChild(item);
	}
	data.list.forEach(entry => {
		const item = document.createElement("li");
		item.textContent = `${entry.name} – ${entry.score}`;
		list.appendChild(item);
	});
}

function updateLetterKeys(data) {
	const wrong = data.text2 || "";
	const right = data.text.replace(/[^A-Z]/g, "");
	document.querySelectorAll(".letter-keys button").forEach(btn => {
		const letter = btn.textContent;
		btn.classList.toggle("wrong", wrong.includes(letter));
		btn.classList.toggle("right", right.includes(letter));
	});
}

function initGamePads() {
	document.querySelectorAll("#game-box .pad[data-key]").forEach(pad => {
		let delayTimer = null;
		let repeatTimer = null;
		const stop = () => {
			clearTimeout(delayTimer);
			clearInterval(repeatTimer);
			delayTimer = null;
			repeatTimer = null;
			pad.classList.remove("pressed");
		};
		pad.addEventListener("pointerdown", event => {
			event.preventDefault();
			const key = Number(pad.dataset.key);
			pad.setPointerCapture(event.pointerId);
			pad.classList.add("pressed");
			sendGameKey(key);
			vibrate(10);
			if (pad.dataset.repeat === "1") {
				delayTimer = setTimeout(() => {
					repeatTimer = setInterval(() => sendGameKey(key), GAME_REPEAT_INTERVAL_MS);
				}, GAME_REPEAT_DELAY_MS);
			}
		});
		["pointerup", "pointercancel", "lostpointercapture"].forEach(type => pad.addEventListener(type, stop));
		pad.addEventListener("contextmenu", event => event.preventDefault());
	});

	// Hangman keyboard
	const keys = document.querySelector(".letter-keys");
	for (let code = 65; code <= 90; code++) {
		const btn = document.createElement("button");
		btn.type = "button";
		btn.textContent = String.fromCharCode(code);
		btn.addEventListener("click", () => sendGameKey(code));
		keys.appendChild(btn);
	}

	// Clock quiz
	const ticks = document.querySelector(".quiz-ticks");
	for (let i = 0; i < 12; i++) {
		const angle = i * 30 * Math.PI / 180;
		const tick = document.createElementNS("http://www.w3.org/2000/svg", "line");
		tick.setAttribute("x1", Math.sin(angle) * 40);
		tick.setAttribute("y1", -Math.cos(angle) * 40);
		tick.setAttribute("x2", Math.sin(angle) * 45);
		tick.setAttribute("y2", -Math.cos(angle) * 45);
		ticks.appendChild(tick);
	}
	const hour = document.getElementById("quiz-hour");
	const minute = document.getElementById("quiz-minute");
	const updateClock = () => {
		const hh = Number(hour.value);
		const mm = Number(minute.value);
		document.getElementById("quiz-hour-hand").setAttribute("transform", `rotate(${(hh % 12) * 30 + mm / 2})`);
		document.getElementById("quiz-minute-hand").setAttribute("transform", `rotate(${mm * 6})`);
		document.getElementById("quiz-time").textContent = `${hh}:${String(mm).padStart(2, "0")}`;
	};
	hour.addEventListener("input", updateClock);
	minute.addEventListener("input", updateClock);
	updateClock();
	document.getElementById("quiz-check").addEventListener("click", () => {
		sendGameKey(GAME_KEY.ANSWER, Number(hour.value) * 100 + Number(minute.value));
	});

	// Mental arithmetic keypad
	document.querySelectorAll(".keypad [data-digit]").forEach(btn => {
		btn.addEventListener("click", () => {
			const digit = btn.dataset.digit;
			if (digit === "back") {
				mathInput = mathInput.slice(0, -1);
			} else if (digit === "ok") {
				if (mathInput) {
					sendGameKey(GAME_KEY.ANSWER, Number(mathInput));
				}
				return;
			} else if (mathInput.length < 4) {
				mathInput += digit;
			}
			document.getElementById("math-display").textContent = mathInput;
		});
	});

	// Paint palette
	const palette = document.getElementById("paint-palette");
	PAINT_COLORS.forEach((color, index) => {
		const swatch = document.createElement("button");
		swatch.type = "button";
		swatch.className = "swatch";
		swatch.style.background = color;
		swatch.setAttribute("aria-label", color);
		swatch.classList.toggle("selected", index === paintColor);
		swatch.addEventListener("click", () => {
			paintColor = index;
			palette.querySelectorAll(".swatch").forEach((el, i) => el.classList.toggle("selected", i === index));
		});
		palette.appendChild(swatch);
	});
	document.getElementById("paint-fill").addEventListener("click", () => sendGameKey(GAME_KEY.PAINT_FILL, paintColor));
	document.getElementById("paint-clear").addEventListener("click", () => sendGameKey(GAME_KEY.PAINT_CLEAR));

	// Tapping the game preview: word find taps letters, paint draws (also by dragging)
	const gamePreview = document.querySelector(".preview-game");
	let painting = false;
	let lastCell = -1;
	const tapCell = target => {
		const cell = target && target.dataset ? Number(target.dataset.cell) : NaN;
		if (Number.isNaN(cell) || cell === lastCell) {
			return;
		}
		lastCell = cell;
		if (currentGame === 20) {
			sendGameKey(GAME_KEY.PAINT_SET, cell * 16 + paintColor);
		} else if (currentGame === 15) {
			sendGameKey(GAME_KEY.TAP, cell);
			vibrate(10);
		}
	};
	gamePreview.addEventListener("pointerdown", event => {
		event.preventDefault();
		painting = true;
		lastCell = -1;
		tapCell(event.target);
	});
	gamePreview.addEventListener("pointermove", event => {
		if (painting && currentGame === 20) {
			tapCell(document.elementFromPoint(event.clientX, event.clientY));
		}
	});
	["pointerup", "pointercancel", "pointerleave"].forEach(type => gamePreview.addEventListener(type, () => { painting = false; }));
	gamePreview.addEventListener("contextmenu", event => event.preventDefault());

	// Keyboard while the games page is open
	const KEY_MAP = new Map([
		["ArrowLeft", GAME_KEY.LEFT],
		["ArrowRight", GAME_KEY.RIGHT],
		["ArrowUp", GAME_KEY.UP],
		["ArrowDown", GAME_KEY.DOWN],
		[" ", GAME_KEY.ACTION],
		["Enter", GAME_KEY.ACTION]
	]);
	document.addEventListener("keydown", event => {
		if (!isSectionVisible("games") || !currentGame || event.target.tagName === "INPUT") {
			return;
		}
		const upper = event.key.length === 1 ? event.key.toUpperCase() : "";
		if (currentGame === 13 && upper >= "A" && upper <= "Z") {
			sendGameKey(upper.charCodeAt(0));
			event.preventDefault();
			return;
		}
		if (currentGame === 3 && upper >= "1" && upper <= "4") {
			sendGameKey(GAME_KEY.COLOR1 + Number(upper) - 1);
			return;
		}
		if (upper === "P") {
			sendGameKey(GAME_KEY.PAUSE);
			return;
		}
		const key = KEY_MAP.get(event.key);
		if (key) {
			event.preventDefault();
			sendGameKey(key);
		}
	});
}

function initGamesAndTimer() {
	const nameInput = document.getElementById("player-name");
	nameInput.value = storageGet("playerName", "");
	nameInput.addEventListener("change", () => storageSet("playerName", nameInput.value));
	const player = storageGet("playerNumber", "1");
	document.querySelectorAll("input[name='player-number']").forEach(radio => {
		radio.checked = radio.value === player;
		radio.addEventListener("change", () => storageSet("playerNumber", radio.value));
	});

	document.querySelectorAll(".tile[data-game]").forEach(tile => {
		tile.addEventListener("click", () => startGame(Number(tile.dataset.game)));
	});
	document.getElementById("game-again").addEventListener("click", () => startGame(currentGame));
	document.querySelectorAll(".game-quit").forEach(btn => {
		btn.addEventListener("click", () => {
			currentGame = 0;
			document.getElementById("game-box").hidden = true;
			sendColorData(CMD.MODE_WORD_CLOCK);
		});
	});
	initGamePads();

	const sendTimer = (action, seconds = 0) => sendCmd(CMD.MODE_TIMER, nstr(action) + nstr5(seconds));
	document.getElementById("timer-countdown").addEventListener("click", () => {
		const minutes = Math.min(99, Math.max(0, Number(document.getElementById("timer-minutes").value) || 0));
		const seconds = Math.min(59, Math.max(0, Number(document.getElementById("timer-seconds").value) || 0));
		const total = minutes * 60 + seconds;
		if (total > 0) {
			sendTimer(TIMER_ACTION.COUNTDOWN, total);
		}
	});
	document.getElementById("timer-stopwatch").addEventListener("click", () => sendTimer(TIMER_ACTION.STOPWATCH));
	document.getElementById("timer-pause").addEventListener("click", () => sendTimer(TIMER_ACTION.PAUSE));
	document.getElementById("timer-stop").addEventListener("click", () => sendTimer(TIMER_ACTION.STOP));

	initPreviews();
	initHome();
	initExtras();
	initAppearance();
	startFramePolling();

	// Opening the page with #games or #extras (e.g. from a home screen icon) goes straight there.
	const target = location.hash.replace("#", "") || "home";
	const link = document.querySelector(`[data-navigation='${target}']`);
	if (link) link.click();
}

function renderTimerStatus(data) {
	const display = document.getElementById("timer-display");
	display.textContent = formatTimer(data.seconds);
	display.classList.toggle("finished", Boolean(data.finished));
	if (data.finished) {
		vibrate([200, 100, 200, 100, 200]);
	}
}

/*
 * Start page
 */
function sendMessage(text) {
	const clean = toClockText(text).trim().substring(0, DATA_SCROLLINGTEXT_LENGTH);
	if (!clean) {
		return;
	}
	const repeats = Number(document.getElementById("message-repeats").value) || 2;
	sendCmd(CMD.MODE_MESSAGE, nstr(repeats) + getPaddedString(clean, DATA_SCROLLINGTEXT_LENGTH));
}

function navigateTo(name) {
	const link = document.querySelector(`.pure-menu-link[data-navigation='${name}']`);
	if (link) link.click();
}

function initHome() {
	document.querySelectorAll(".tile[data-goto]").forEach(tile => {
		tile.addEventListener("click", () => {
			navigateTo(tile.dataset.goto);
			if (tile.dataset.scroll) {
				document.getElementById(tile.dataset.scroll).scrollIntoView({ behavior: "smooth" });
			}
		});
	});
	document.querySelectorAll(".tile[data-action='clock']").forEach(tile => {
		tile.addEventListener("click", () => sendColorData(CMD.MODE_WORD_CLOCK));
	});
	document.querySelectorAll(".tile[data-ambient]").forEach(tile => {
		tile.addEventListener("click", () => sendCmd(CMD.MODE_AMBIENT, nstr(Number(tile.dataset.ambient))));
	});
	document.getElementById("message-send").addEventListener("click", () => {
		sendMessage(document.getElementById("message-text").value);
	});
	document.querySelectorAll("[data-message]").forEach(btn => {
		btn.addEventListener("click", () => sendMessage(btn.dataset.message));
	});
	document.getElementById("alarm-stop").addEventListener("click", () => {
		sendColorData(CMD.MODE_WORD_CLOCK);
		document.getElementById("alarm-ringing").hidden = true;
	});
}

/*
 * Alarms and word colors
 */
const ALARM_ACTIVE = 0x80;
const ALARM_EFFECTS = [[7, "extras.alarm.sunrise"], [8, "extras.alarm.flash"], [4, "home.mood-fireworks"],
	[1, "home.mood-lava"], [2, "home.mood-fire"], [5, "home.mood-stars"]];
// Monday first; bit 0 is Sunday on the clock
const WEEKDAY_BITS = [1, 2, 3, 4, 5, 6, 0];

function createAlarmRow(index) {
	const row = document.createElement("div");
	row.className = "alarm-row";
	row.innerHTML = `
		<label class="switch"><input type="checkbox" class="alarm-active"><span class="slider round"></span></label>
		<input type="time" class="alarm-time" value="07:00" aria-label="time">
		<span class="alarm-days"></span>
		<select class="alarm-effect" aria-label="effect"></select>`;
	const days = row.querySelector(".alarm-days");
	WEEKDAY_BITS.forEach((bit, i) => {
		const label = document.createElement("label");
		const box = document.createElement("input");
		box.type = "checkbox";
		box.dataset.bit = bit;
		box.checked = i < 5;
		label.appendChild(box);
		label.appendChild(document.createTextNode(i18next.t(`extras.alarm.day${i}`)));
		days.appendChild(label);
	});
	const effect = row.querySelector(".alarm-effect");
	ALARM_EFFECTS.forEach(([value, key]) => {
		const option = document.createElement("option");
		option.value = value;
		option.textContent = i18next.t(key);
		effect.appendChild(option);
	});
	row.dataset.index = index;
	return row;
}

function renderAlarms(alarms) {
	const list = document.getElementById("alarm-list");
	if (!list.children.length) {
		for (let i = 0; i < 4; i++) {
			list.appendChild(createAlarmRow(i));
		}
	}
	(alarms || []).forEach(([flags, hour, minute, effect], i) => {
		const row = list.children[i];
		if (!row) return;
		row.querySelector(".alarm-active").checked = (flags & ALARM_ACTIVE) !== 0;
		row.querySelector(".alarm-time").value = `${String(hour).padStart(2, "0")}:${String(minute).padStart(2, "0")}`;
		row.querySelectorAll(".alarm-days input").forEach(box => {
			box.checked = (flags & (1 << Number(box.dataset.bit))) !== 0;
		});
		row.querySelector(".alarm-effect").value = effect || 7;
	});
}

function sendAlarms() {
	let payload = "";
	document.querySelectorAll("#alarm-list .alarm-row").forEach(row => {
		let flags = row.querySelector(".alarm-active").checked ? ALARM_ACTIVE : 0;
		row.querySelectorAll(".alarm-days input").forEach(box => {
			if (box.checked) flags |= 1 << Number(box.dataset.bit);
		});
		const [hour, minute] = (row.querySelector(".alarm-time").value || "07:00").split(":").map(Number);
		payload += nstr(flags) + nstr(hour) + nstr(minute) + nstr(Number(row.querySelector(".alarm-effect").value));
	});
	sendCmd(CMD.SET_ALARMS, payload);
}

function initExtras() {
	renderAlarms([]);
	document.getElementById("notify-test").addEventListener("click", () => {
		const text = toClockText(document.getElementById("notify-text").value).substring(0, DATA_SCROLLINGTEXT_LENGTH);
		sendCmd(CMD.SHOW_NOTIFICATION, nstr(Number(document.getElementById("notify-icon").value)) + nstr(3) +
			getPaddedString(text, DATA_SCROLLINGTEXT_LENGTH));
	});
	document.getElementById("alarm-save").addEventListener("click", sendAlarms);
	document.querySelectorAll(".word-color-option").forEach(el => {
		el.addEventListener("change", () => {
			const flags = (document.getElementById("learn-colors").checked ? 1 : 0) |
				(document.getElementById("new-year").checked ? 2 : 0);
			sendCmd(CMD.SET_WORD_COLORS, nstr(flags));
		});
	});
}

function renderExtrasConfig(data) {
	if (data.alarms) {
		renderAlarms(data.alarms);
	}
	if (data.wordColorFlags !== undefined) {
		document.getElementById("learn-colors").checked = (data.wordColorFlags & 1) !== 0;
		document.getElementById("new-year").checked = (data.wordColorFlags & 2) !== 0;
	}
}

/*
 * Appearance: dark mode and expert settings
 */
function applyTheme(theme) {
	if (theme === "auto") {
		document.documentElement.removeAttribute("data-theme");
	} else {
		document.documentElement.setAttribute("data-theme", theme);
	}
}

function initAppearance() {
	const theme = document.getElementById("theme");
	theme.value = storageGet("theme", "auto");
	applyTheme(theme.value);
	theme.addEventListener("change", () => {
		storageSet("theme", theme.value);
		applyTheme(theme.value);
	});
	const expert = document.getElementById("expert-mode");
	expert.checked = storageGet("expertMode", "0") === "1";
	document.body.classList.toggle("expert", expert.checked);
	expert.addEventListener("change", () => {
		storageSet("expertMode", expert.checked ? "1" : "0");
		document.body.classList.toggle("expert", expert.checked);
	});
}

/*
 * Digital clock options, see DIGITAL_* in WordClockState.h
 */
const DIGITAL = { SEPARATOR_MASK: 0x03, H12: 0x04, NO_LEADING_ZERO: 0x08, SECONDS_BAR: 0x10, CENTERED: 0x20 };

function getDigitalClockFlags() {
	let flags = Number(document.getElementById("digital-separator").value) & DIGITAL.SEPARATOR_MASK;
	if (document.getElementById("digital-12h").checked) flags |= DIGITAL.H12;
	if (!document.getElementById("digital-leading-zero").checked) flags |= DIGITAL.NO_LEADING_ZERO;
	if (document.getElementById("digital-seconds-bar").checked) flags |= DIGITAL.SECONDS_BAR;
	if (document.getElementById("digital-centered").checked) flags |= DIGITAL.CENTERED;
	return flags;
}

function setDigitalClockOptions(flags) {
	document.getElementById("digital-separator").value = flags & DIGITAL.SEPARATOR_MASK;
	document.getElementById("digital-12h").checked = Boolean(flags & DIGITAL.H12);
	document.getElementById("digital-leading-zero").checked = !(flags & DIGITAL.NO_LEADING_ZERO);
	document.getElementById("digital-seconds-bar").checked = Boolean(flags & DIGITAL.SECONDS_BAR);
	document.getElementById("digital-centered").checked = Boolean(flags & DIGITAL.CENTERED);
}
