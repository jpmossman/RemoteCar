(() => {

    class Joystick extends EventTarget {

        constructor(elem = null, options = {}) {
            super();
            this.elem = elem ? elem : Joystick.createNewElement();
            // this.mode = options.mode || 'velocity';
            // this.sendPath = options.sendTo;
            this.sendInt = (options.sendInterval > 50) ? options.sendInterval : 50;
            this.debug = options.debug == true;
            this.enabled = false;
            this.initialized = false;
            this.prevX = 0; // prev values used to indicate the set of xy that is processed and sent to server
            this.prevY = 0;
            this.currX = 0;
            this.currY = 0;
            this._lastDispatch = 0;
            this.cbs = { // callbacks
                tStart: this._handleTouch.bind(this),
                tMove: this._handleMove.bind(this),
                tEnd: this._handleRelease.bind(this)
            }
        }

        static createNewElement() {
            let joystick = document.createElement('div');
            joystick.setAttribute('class', 'joystick');
            joystick.appendChild(document.createElement('div'));
            return joystick;
        }

        enable() {
            if (!this.initialized) return;
            this.enabled = true;
            this.elem.classList.remove('disabled');
        }

        disable() {
            this.enabled = false;
            this.elem.classList.add('disabled');
        }

        init() {
            if (this.initialized) return;
            this.elem.addEventListener('touchstart', this.cbs.tStart, { passive: false });
            this.elem.addEventListener('touchmove', this.cbs.tMove, { passive: false });
            this.elem.addEventListener('touchend', this.cbs.tEnd);

            this.initialized = true;

            this._stateCheckLoop();

            this.enable();
        }

        disconnect() {
            if (this.initialized) {
                this.elem.removeEventListener('touchstart', this.cbs.tStart);
                this.elem.removeEventListener('touchmove', this.cbs.tMove);
                this.elem.removeEventListener('touchend', this.cbs.tEnd);
            }

            this.initialized = false;

            this.disable();
        }

        _handleTouch(e) {
            e.preventDefault();
            if (!this.enabled) return;

            let touch = e.touches[0] || e.changedTouches[0];
            this._updatePositionData(touch.pageX - this.elem.offsetLeft - this.elem.offsetWidth / 2, touch.pageY - this.elem.offsetTop - this.elem.offsetWidth / 2);
            this.elem.classList.remove('released');
            this._repositionStick();
            this._dispatchState();
        }

        _handleMove(e) {
            e.preventDefault();
            if (!this.enabled) return;

            let touch = e.touches[0] || e.changedTouches[0];
            this._updatePositionData(touch.pageX - this.elem.offsetLeft - this.elem.offsetWidth / 2, touch.pageY - this.elem.offsetTop - this.elem.offsetWidth / 2);
            this._repositionStick();
        }

        _handleRelease(e) {
            e.preventDefault();
            if (!this.enabled) return;

            this._updatePositionData(0, 0);
            this.elem.classList.add('released');
            this._repositionStick();
            this._dispatchState();
        }

        _updatePositionData(x = 0, y = 0) {
            if (x != this.currX) {
                this.currX = x;
            }
            if (y != this.currY) {
                this.currY = y;
            }
        }

        _repositionStick() {
            let stick = this.elem.children[0];
            if (!stick) return;
            let rad = Math.atan2(this.currY, this.currX);
            let mag = Math.hypot(this.currX, this.currY);
            if (mag > this.elem.offsetWidth / 2) {
                stick.style.left = (this.elem.offsetWidth * (Math.cos(rad) + 1) - stick.offsetWidth) / 2 + 'px';
                stick.style.top = (this.elem.offsetWidth * (Math.sin(rad) + 1) - stick.offsetHeight) / 2 + 'px';
            }
            else {
                stick.style.left = (this.currX + (this.elem.offsetWidth - stick.offsetWidth) / 2) + 'px';
                stick.style.top = (this.currY + (this.elem.offsetWidth - stick.offsetHeight) / 2) + 'px';
            }
        }

        _processPositionData() {
            this.prevX = this.currX;
            this.prevY = this.currY;
            let px, py, rad, mag;
            rad = Math.atan2(this.currY, this.currX);
            mag = Math.hypot(this.currX, this.currY);
            if (mag > this.elem.offsetWidth / 2) { // if user drags finger further than joystick bounds 
                px = Math.round(this.currX / mag * 100);
                py = Math.round(this.currY / mag * 100);
                return { rad: 2 * Math.PI - (Math.sign(rad) == -1 ? rad + 2 * Math.PI : rad), mag: 100 }; // 100% magnitude; max magnitude is radius of bounding circle
            }
            else { // if user drags finger within bounds
                px = Math.round(this.currX * 2 / this.elem.offsetWidth * 100);
                py = Math.round(this.currY * 2 / this.elem.offsetWidth * 100);
                return { rad: 2 * Math.PI - (Math.sign(rad) == -1 ? rad + 2 * Math.PI : rad), mag: mag * 2 / this.elem.offsetWidth * 100 }; // magnitude normalized to a percent value
            }
        }

        _dispatchState(ignoreLastDispatch = false) {
            if (this.prevX != this.currX || this.prevY != this.currY) {
                let now = Date.now();
                if (ignoreLastDispatch || now > this._lastDispatch + this.sendInt) {
                    this._lastDispatch = now;
                    let polarCoords = this._processPositionData();
                    // let uri;
                    // if (this.sendPath) {
                    //     uri = `${this.sendPath}?mode=${this.mode}&x=${pCoords.x}&y=${pCoords.y}`;
                    //     fetch(uri, {
                    //         method: 'POST'
                    //     });
                    // }

                    // if (this.debug) {
                    //     if (uri) {
                    //         console.log(`[ID: '${this.elem.id}']: StickChange (POST '${uri}')`);
                    //     }
                    //     else {
                    //         console.log(`[ID: '${this.elem.id}']: StickChange (x: ${pCoords.x}, y: ${pCoords.y})`);
                    //     }
                    // }
                    this.dispatchEvent(new CustomEvent('move', { detail: polarCoords }));
                }
            }
        }

        _stateCheckLoop() {
            if (!this.initialized) return; // this line will exit loop when disconnected
            this._dispatchState(true);
            setTimeout(this._stateCheckLoop.bind(this), this.sendInt);
        }
    }

    class InputAnalyzer {

        constructor(inputObject, size) {
            this.elem = document.createElement('div');
        }
    }

    // try {
    //     var socket = new WebSocket('ws://' + location.hostname + ':3004/', ['arduino']);
    // }
    // catch {
    //     window.alert('Warning: Unable to communicate with microcontroller');
    // }

    if (window.location.protocol == 'http:' || window.location.protocol == 'https:') {
        var socket = io();
    }
    else {
        var socket = null;
    }


    document.addEventListener('DOMContentLoaded', (e) => {
        let js = new Joystick();
        document.body.appendChild(js.elem);
        js.init();
        js.addEventListener('move', (e) => {
            let data = { rad: Math.floor(e.detail.rad * 255 / (2 * Math.PI)), mag: Math.floor(e.detail.mag * 255 / 100) };
            console.log(data);
            socket?.emit('input/joystick', data);
        });
    });

})();