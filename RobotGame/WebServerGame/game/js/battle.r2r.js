/**
* ロボット間通信クラス。
* @class BattleR2R
* @constructor
* @param {json} config 設定情報。
* @param {String} config[i] 自分のIPアドレス。設定されなかった場合、window.location.hostnameを使用する
* @param {String} config[you] 通信相手のIPアドレス
* @param {function} cb コールバック
* @example
* <pre> <code>
* var r2r = new BattleR2R({'i':'192.168.11.2','you':'192.168.11.4'},
*                         function callback(msg){
*	                         console.log(msg);
*                         });
* var r2r = new BattleR2R({'you':'192.168.11.4'},
*                         function callback(msg){
*	                         console.log(msg);
*                         });
* </code></pre>
**/
var BattleR2R = function(config,cb){
	this.ip = null;
	if(config && config['i']) {
		this.ip = config['i'];
	} else {
		this.ip = window.location.hostname;
	}
	this.ws = null;
	this.cb = cb;
	this.ready = false;

	this.yourIP = null;

	if(config && config['you']) {
		this.yourIP = config['you'];
	} else {
		console.error('Please give me a dist robot.');
	}

	function openWS(self,ip) {
		self.ws = new WebSocket('ws://' + ip +':20088','r2r-comm');
		self.ws.onerror = function (error) {
		};
		// Log messages from the server
		self.ws.onmessage = function (evt) {
			var remoteMsg = JSON.parse( evt.data );
			if(self.cb){
				self.cb(remoteMsg);
			}
		};
		self.ws.onopen = function (evt) {
			self.ready = true;
		};
		self.ws.onclose = function(evt){
			var remoteMsg = JSON.parse( evt.data );
			self.ready = false;
		}
	};
	openWS(this,this.ip);	


	
	/**
	* メッセージを送信する。
	* @method send
	* @param {json} msg メッセージ
    * @example
    * <pre><code>
	* function listen(msg) {
	*	console.log(msg);
	* }
    * var r2r = new BattleR2R({'i':'192.168.11.2','you':'192.168.11.4'},listen);
	* var msg = {'attack':{}};
	* r2r.send(msg);
	* </code></pre>
	**/
	this.send = function(msg) {
		var r2rMsg ={'r2r':msg};
		r2rMsg['dist'] = this.yourIP;
		if(null == this.ws && false ==this.ready){
			openWS(this,this.ip);
		}
		else
		{
			this.ws.send(JSON.stringify(r2rMsg));
		}
	};
}
