/**
* V-Sido CONNECTとの通信クラス。
* @class VSidoWeb
* @constructor
* @param {json} config 設定情報、IPアドレスのみ。
* 設定されなかった場合、window.location.hostnameを使用する。
* @example
* <pre> <code>
* var vsido = new VSidoWeb({'ip':'192.168.11.2'});
* var vsido = new VSidoWeb();
* </code></pre>
**/
var VSidoWeb = function(config){
	if(config && config['ip']) {
		this.ip = config['ip'];
	}
	else {
		this.ip = window.location.hostname;
	}
	this.ws = null;
	this.cb = null;
	this.ready = false;
	this.requestDate = new Date();

	// open websocket 
	function openWS(self) {
		self.ws = new WebSocket('ws://' + self.ip +':20080','vsido-cmd');
		self.ws.onerror = function (error) {
			console.error(error);
		};
		// Log messages from the server
		self.ws.onmessage = function (evt) {
			var remoteMsg = JSON.parse( evt.data );
			var responseDate = new Date();
			var timeResponse = responseDate.getTime() - self.requestDate.getTime();
			if(self.cb){
				self.cb(remoteMsg,timeResponse);
			}
		};
		self.ws.onopen = function (evt) {
			self.ready = true;
		};
		self.ws.onclose = function(evt){
			var remoteMsg = JSON.parse( evt.data );
			self.ready = false;
		}
	}
	openWS(this,this.ip);
	
	/**
	* V-Sido CONNECTにコマンドを送信する。
	* @method send
	* @param {json} 　　　　cmd コマンド
	* @param {function} 　cb 返事のコールバック
    * @example
    * <pre><code>
	* var angle = {"cmd":"servoAngle","cycle":1,"servo":[{"sid":2,"angle":-26}]};
	* vsido.send(angle,function(response){
	*   console.log(JSON.stringify(response));
	* });
	* </code></pre>
	**/
	this.send = function(cmd,cb) {
		this.requestDate = new Date();
		this.cb = cb;
		if(null != this.ws && false!=this.ready) {
			this.ws.send(JSON.stringify(cmd));
		}
		else{
			openWS(this,this.ip)
		}
	};

	
	
	/**
	* 目標角度設定コマンドを生成する。
	* @method servoAngle
    * @example
    * <pre><code>
	* var angle = vsido.servoAngle();
	* angle["cycle"] = 2;
	* angle["servo"].push({"sid":2,"angle":100});
	* </code></pre>
	**/
	this.servoAngle = function() {
		var jsonMsg = {
			"cmd": "servoAngle",
			"cycle": 1,/*step*/
			"servo":[
				/*
				array of {'sid':1-127,'angle':-140~+140}
				*/
			]
		};
		return jsonMsg;
	}

	/**
	* サーボ情報読み込みコマンドを生成する。
	* @method readServoInfo
    * @example
    * <pre><code>
	* var info = vsido.readServoInfo();
	* info["servo"].push({"sid":2,"address":100,"length": 2});
	* </code></pre>
	**/
	this.readServoInfo = function() {
		var jsonMsg = {
			"cmd": "servo_info",
			"servo":[
			/*
				see this.servoInfo
			*/
			]
		};
		return jsonMsg;
	}
	
	/**
	* サーボ情報読み込みコマンド用のパラメータを生成する。
	* 現在角度読み込むためパラメータ。
	* @method readServoAngle
    * @example
    * <pre><code>
	* var info = vsido.readServoInfo();
	* var readAngle = vsido.readServoAngle();
	* info["servo"].push(readAngle);
	* </code></pre>
	**/
	this.readServoAngle = function() {
		var jsonMsg = {
			"sid": 2,
			"address": 19,
			"length": 2,
		};
		return jsonMsg;
	}

	
	/**
	* IK設定コマンドを生成する。
	* @method ik
    * @example
    * <pre><code>
	* var ik = vsido.ik();
	* ik['ikf']['dist']['pos'] = true;
	* var kdt = vsido.kdt();
	* ik["kdts"].push(kdt);
	* </code></pre>
	**/
	this.ik = function() {
		var jsonMsg = {
			"cmd": "ik",
			"ikf": {
			  "dist"    :{"torq":false,"pos":false,"rot":false}
			},
			"kdts":[
			/*
				see this.kdt
			*/
			]
		};
		return jsonMsg;
	}

	/**
	* IK設定コマンドのパラメータを生成する。
	* @method kdt
    * @example
    * <pre><code>
	* var ik = vsido.ik();
	* ik['ikf']['dist']['pos'] = true;
	* var kdt = vsido.kdt();
	* kdt['kid'] = 2;
	* kdt['kdt']['pos']['x'] = 35;
	* kdt['kdt']['pos']['y'] = 21;
	* kdt['kdt']['pos']['z'] = 68;
	* ik["kdts"].push(kdt);
	* </code></pre>
	**/
	this.kdt = function() {
		var jsonMsg = {
			"kid":0,/* 0~15 */
			"kdt": {
			  "pos"    :{"x":0,"y":0,"z":0},/* -100 ~ 100 percentage */
			  "rot"    :{"Rx":0,"Ry":0,"Rz":0},/* -100 ~ 100 percentage */
			  "torq"   :{"Tx":0,"Ty":0,"Tz":0},/* -100 ~ 100 percentage */
			}
		};
		return jsonMsg;
	}

	/**
	* IK読み込みコマンドを生成する。 
	* @method readIK
    * @example
    * <pre><code>
	* var ik = vsido.readIK();
	* ik['ikf']['cur']['pos'] = true;
	* ik['kids'].push(2);
	* </code></pre>
	**/
	this.readIK = function() {
		var jsonMsg = {
			"cmd": "ik",
			"ikf": {
			  "cur" :{"torq":false,"pos":false,"rot":false}
			},
			"kids":[
				/*
					0~15
				*/
			]
		};
		return jsonMsg;
	}
	

	/**
	* 歩行設定コマンドを生成する。 
	* @method walk
    * @example
    * <pre><code>
	* var walk = vsido.walk();
	* walk['forward']= 50;
	* walk['turn'] = 0;
	* </code></pre>
	**/
	this.walk = function() {
		var jsonMsg = {
			"cmd": "walk",
			"forward":0,/* -100 ~ 100 percentage */
			"turn":0/* -100 ~ 100 percentage */
		};
		return jsonMsg;
	}

	/**
	* RAW コマンド(pass through)を生成する。
	* RAW コマンドはV-Sido CONNECTにそのまま送られる。
	* @method execRaw
    * @example
    * <pre><code>
	* var raw = vsido.execRaw();
	* raw['exec'].push(0xff);
	* raw['exec'].push(0x6f);
	* raw['exec'].push(0x08);
	* raw['exec'].push(0x01);
	* raw['exec'].push(0x02);
	* raw['exec'].push(0xc8);
	* raw['exec'].push(0x00);
	* raw['exec'].push(0x53);
	* </code></pre>
	**/
	this.execRaw = function() {
		var jsonMsg = {
			"cmd": "raw",
			"exec" :[
			/*
			 Hex data spilted by [,]
			 example 0xff,0x74,0x4,0xaa
			*/
			]
		};
		return jsonMsg;
	}

	/**
	* Bluetoothデバイスをスキャンする。
	* @method scanBT
	* @param {function} 　cb デバイス名、Macアドレス一覧
    * @example
    * <pre><code>
	* vsido.scanBT(function(response){
	*   console.log(JSON.stringify(response));
	* });
	* </code></pre>
	**/
	this.scanBT = function(cb) {
		this.cb = cb;
		var scanMsg = {'cmd':'scan_bt'};
		if(null != this.ws && false!=this.ready) {
			this.ws.send(JSON.stringify(scanMsg));
		}
		else{
			openWS(this,this.ip)
		}
	}

	/**
	* Bluetoothデバイスをぺアリングする。 
	* @method bindBT
	* @param {json} device Macアドレス
    * @example
    * <pre><code>
	* var device = {'mac':'00:1B:DC:09:53:C3'};
	* vsido.bindBT(device);
	* </code></pre>
	**/
	this.bindBT = function(device) {
		var bindMsg = {'cmd':'bind_bt','device':device};
		if(null != this.ws && false!=this.ready) {
			this.ws.send(JSON.stringify(bindMsg));
		}
		else{
			openWS(this,this.ip)
		}
	}
};
