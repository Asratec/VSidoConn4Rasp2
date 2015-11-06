/**
* USBカメラクラス。
* @class Camera
* @constructor
* @param {json} config 設定情報、IPアドレスのみ。 
* 設定されなかった場合、window.location.hostnameを使用する。
* @example
* <pre> <code>
* var camera = new Camera({'ip':'192.168.11.2'});
* var camera = new Camera();
* </code></pre>
**/
var Camera = function(config){
	this.ip = null;
	if(config && config['ip']) {
		this.ip = config['ip'];
	}
	else {
		this.ip = window.location.hostname;
	}
	
	/**
	* カメラ画像を表示する。
	* @method viewRaw
	* @param {img} 		raw 	image要素
    * @example
    * <pre><code>
	* var camera = new Camera({'ip':'192.168.11.2'});
    * camera.viewRaw($('#camera_raw'));	
	* </code></pre>
	* </code></pre>
	**/
	this.viewRaw = function(raw) {
		var random = Math.floor(Math.random() * Math.pow(2, 31));
		raw.attr('src', 'http://'+ this.ip +':18081/raw?i=' + Math.random());
	}
	/**
	* マーカー検出結果画像を表示する。
	* @method viewMarkerDetect
	* @param {img} 		marker 	image要素
    * @example
    * <pre><code>
	* var camera = new Camera({'ip':'192.168.11.2'});
    * camera.viewMarkerDetect($('#camera_marker'));	
	* </code></pre>
	**/
	this.viewMarkerDetect = function(marker,ip) {
		var random = Math.floor(Math.random() * Math.pow(2, 31));
		marker.attr('src', 'http://'+ this.ip +':18083/marker?i=' + Math.random());
	}

	
	this.ws = null;
	this.cb = null;
	this.ready = false;
	function openWS(self) {
		self.ws = new WebSocket('ws://' + self.ip +':20088','webcam-info');
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
			self.ws = null;
		}
	};
	openWS(this,this.ip);
	
	/**
	* マーカー検出通知を受け取る。
	* @method listen
	* @param {function} cb 通知のコールバック
    * @example
    * <pre><code>
	* var camera = new Camera({'ip':'192.168.11.2'});
	* camera.listen(function(msg){
    *   console.log(msg);
    * });
	* </code></pre>	
	*/
	this.listen = function(cb) {
		this.cb = cb;
		if(this.ready && null!=this.ws){
		}
		else
		{
			openWS(this);
		}
	};
	
	/**
	* マーカー色を設定する。HSV色空間。
	* @method setMarker
	* @param {json} color 色
    * @example
    * <pre><code>
	* var camera = new Camera({'ip':'192.168.11.2'});
	* // 緑マーカー
	* var color = {
	* 	'HueL':0x25, /// 色相のしきい値-下限[0x0,0xFF]
	* 	'HueU':0x3D, /// 色相のしきい値-上限[0x0,0xFF]
	* 	'SatL':0x50, /// 彩度のしきい値-下限[0x0,0xFF]
	* 	'SatU':0xFF, /// 彩度のしきい値-上限[0x0,0xFF]
	* 	'ValL':0x93, /// 明度のしきい値-下限[0x0,0xFF]
	* 	'ValU':0xFF  /// 明度のしきい値-上限[0x0,0xFF]
	* };
	* camera.setMarker(color);
	* </code></pre>	
	*/
	this.setMarker = function(color) {
		var marker ={'markerColor':color};
		if(this.ready && null!=this.ws){
			this.ws.send(JSON.stringify(marker));
		}
		else
		{
			openWS(this);
		}
	};
}
