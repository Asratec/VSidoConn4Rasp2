/**
* V-Sido CONNECTとの通信機能。
* @namespace vsido
**/
var vsido = vsido || {
	SERVO_INFO_ITEM:[
		'all',						/* すべての項目 */
		'rom_model_num',			/* サーボモデル番号 −32768~32767 */
		'rom_servo_ID',				/* SID 1～254 */
		'rom_cw_agl_lmt',			/* 時計回り回転リミット角（deg） -180.0~180.0 */
		'rom_ccw_agl_lmt',			/* 反時計回り回転リミット角（deg） -180.0~180.0 */
		'rom_damper',				/* ダンパー 0～254 */
		'rom_cw_cmp_margin',		/* コンプライアンスマージン（deg） 0.0～25.4 */
		'rom_ccw_cmp_margin',		/* コンプライアンスマージン（deg） 0.0～25.4 */
		'rom_cw_cmp_slope',			/* コンプライアンススコープ（deg） 0.0～25.4 */
		'rom_ccw_cmp_slope',		/* コンプライアンススコープ（deg） 0.0～25.4 */
		'rom_punch',				/* パンチ（最大トルクの 0.01%単位） 0～254 */
		'ram_goal_pos',				/* 目標角度 -180.0~180.0 */
		'ram_goal_tim',				/* 速度（目標到達時間を 10msec 刻みで指定） −32768~32767 */
		'ram_max_torque',			/* 最大トルク 0~254 */
		'ram_torque_mode',			/* トルクモード 0:off、1:on、2:break */
		'ram_pres_pos',				/* 現在角度（deg） -180.0~180.0 */
		'ram_pres_time',			/* 現在時間（移動コマンド受信から 10msec 刻み） −32768~32767 */
		'ram_pres_spd',				/* 現在速度（参考程度） −32768~32767 */
		'ram_pres_curr',			/* 現在電流（mA） −32768~32767 */
		'ram_pres_temp',			/* 現在温度（℃） −32768~32767 */
		'ram_pres_volt',			/* 現在電圧（10mV） −32768~32767 */
		'Flags',					/* サーボのリターンフラグ（温度エラーなど） 0～254 */
		'alg_ofset',				/* トリム角（deg） −32768~32767 */
		'parents_ID',				/* ダブルサーボ時の ID 1～254 */
		'connected',				/* サーボ接続の有無（0:なし、1:あり） */
		'read_time',				/* 関節角度の受信にかかった時間（msec） −32768~32767 */
		'_ram_goal_pos',			/* 前回の目標角度（deg） -180.0~180.0 */
		'__ram_goal_pos',			/* 前々回の目標角度（deg） -180.0~180.0 */
		'_ram_res_pos',				/* 前回の現在角度（deg） -180.0~180.0 */
		'_send_speed',				/* 前回の目標速度（0.1deg/sec） 0～254 */
		'_send_cmd_time',			/* 前回の long packet コマンド送信時間（10msec） 0～254 */
		'flg_min_max',				/* 現在角＞最大角のとき 1 現在角＜最小角のとき 1 通常 0 */
		'flg_goal_pos',				/* 目標角度に変化があったとき 1、変化がないとき 0 */
		'flg_parent_inv',			/* ダブルサーボ時の逆転（実装予定） */
		'flg_cmp_slope',			/* コンプライアンススロープに変化があったとき 1、変化がないとき 0 */
		'flg_check_angle',			/* 常に角度情報を監視するか否か   0:監視しない  1:監視する */
		'port_type',				/* TTL 接続のとき 1、RS485 接続のとき 0 */
		'servo_type'				/* サーボメーカー  0：モデルなし   1：FUTABA  2：ROBOTIS（Dynamixel Communication1.0）  3：ROBOTIS（Dynamixel Communication2.0） */
	],
	VID_ITEM:[
		'all',						/* すべての項目 */
		'RS485_Baudrate',			/* RS485 接続のサーボとの通信 速度（単位：bps）  0：B_9600  1：B_57600  2：B_115200  3：B_1000000 */
		'TTL_Baudrate',				/* TTL 接続サーボとの通信速度 速度（単位：bps）  0：B_9600  1：B_57600  2：B_115200  3：B_1000000 */
		'RS232_Baudrate',			/* 外部端末と RS232 接続の通信 速度（単位：bps）  0：B_9600  1：B_57600  2：B_115200  3：B_1000000 */
		'IO_PA_IO_Mode',			/* 汎用端子の IO モード */
		'IO_PA_Analog_Mode',		/* 汎用端子のアナログモード */
		'IO_PA_PWM',				/* 汎用端子の PWM モード 0 or 1 */
		'IO_PA_PWM_CYCLE',			/* 汎用端子のPWM周期 0～65532 usec */
		'Through_Port',				/* パススルーポート */
		'Servo_Type_RS485',			/* RS485 サーボの種類  0：なし  1：FUTABA   2：ROBOTIS ver1.0   3：ROBOTIS ver2.0 */
		'Servo_Type_TTL',			/* TTL サーボの種類  0：なし  1：FUTABA   2：ROBOTIS ver1.0   3：ROBOTIS ver2.0 */
		'IMU_Type',					/* IMU の種類  0：なし  1：センサスティック  2：MPU9150 */
		'Balancer_Flag',			/* オートバランサーの ON/OFF  0:OFF 1:ON */
		'Theta_Th',					/* 角度の閾値 0.1～10.0 */
		'Cycletime',				/* 実行時間 1～100 10msec */
		'Min_Cmp',					/* 最小コンプライアンス(deg) 1～250 */
		'Flag_Ack',					/* Ack フラグの有無 0～1 */
		'Volt_Th',					/* 電圧の閾値(未実装) 6.0～9.0 v */
		'Initialize_Torque',		/* 初期化時のトルク有無 0～1 */
		'Initialize_Angle',			/* 初期化時の目標角度設定  0～1 */
		'Inspection_Flag',			/* 定期診断の有無  0～1 */
		'Inspection_Type',			/* 定期診断の挙動  0～1 */
		'Robot_Model',				/* ロボットモデル  0:モデルなし※2   1:GR-001   2:DARWIN-MINI */
		'UID_Flag'					/* ユーザー設定領域の使用  0:使用しない  1:使用する */
	],
	VID_VALUE:[
		'B_9600',					/* 通信 速度 */
		'B_57600',					/* 通信 速度 */
		'B_115200',					/* 通信 速度 */
		'B_1000000',				/* 通信 速度 */
		'None',						/* RS485 サーボの種類 */
		'FUTABA',					/* RS485 サーボの種類 */
		'ROBOTIS_v1.0',				/* RS485 サーボの種類 */
		'ROBOTIS_v2.0',				/* RS485 サーボの種類 */
		'None',						/* ロボットモデル */
		'GR-001',					/* ロボットモデル */
		'DARWIN-MINI'				/* ロボットモデル */
	],
	KID_ITEM:[
		'body',						/* 体幹 */
		'head',						/* 頭部 */
		'right_hand',				/* 右手 */
		'left_hand',				/* 左手 */
		'right_foot',				/* 右足 */
		'left_foot'					/* 左足 */
	]
};

/**
* V-Sido CONNECTと接続する。
* @class Connect
* @constructor
* @param {object} config 接続設定情報
* <pre><code>
* {'ip':'address String of ip'};
* </code></pre>
* @example
* <pre><code>
* var connect = new vsido.Connect({'ip':'192.168.11.5'});
* </code></pre>
**/
vsido.Connect = function(config){
	this.ip = window.location.hostname;
	this.ws = null;
	this.cb = null;
	this.VSidoWebNotifyCallBack = null;
	this.ready = false;
	this.requestDate = new Date();
	this.waitCmd = null;
	this.openWS = function(self) {
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
			if(self.VSidoWebNotifyCallBack && remoteMsg['type'] == 'StatusNotify'){
				self.VSidoWebNotifyCallBack(remoteMsg);
			}
		};
		self.ws.onopen = function (evt) {
			self.ready = true;
			if(self.waitCmd){
				self.send();
				self.waitCmd = null;
			}
		};
		self.ws.onclose = function(evt){
			var remoteMsg = JSON.parse( evt.data );
			self.ready = false;
			self.ws = null;
		}
	};
	if(config && config['ip']) {
		this.ip = config['ip'];
	}
	this.openWS(this);
};

/**
* V-Sido CONNECTにコマンドを送信する。
* @method send
* @param {object} 　　　　cmd コマンド
* @param {function} 　cb 返事のコールバック
* @example
* <pre><code>
* var connect = new vsido.Connect({'ip':'192.168.11.5'});
* var angle =  new vsido.SetServoAngle(15);
* angle.addAngle(2,100.5);
* connect.send(angle,function(response){
*   console.log(JSON.stringify(response));
* });
* </code></pre>
**/
vsido.Connect.prototype.send = function(cmd,cb){
	this.requestDate = new Date();
	this.cb = cb;
	if(null != this.ws && false != this.ready) {
		this.ws.send(JSON.stringify(cmd));
	}
	else{
		this.openWS(this);
		this.waitCmd = cmd;
	}
};



/**
* 目標角度設定コマンドを生成する。
* @class SetServoAngle
* @constructor
* @param {int} cycle　目標角度に移行するまでの時間 指定範囲 10～1000 単位 ms 精度 10ms
* @example
* <pre><code>
* var angle =  new vsido.SetServoAngle(10);
* </code></pre>
**/
vsido.SetServoAngle = function (cycle){
	this.command='SetServoAngle';
	if('number' == typeof cycle && cycle >= 1  && cycle <= 100) {
		this.cycle= cycle;
	}else{
		this.cycle= 10;
	}
	this.servo = new Array();
};

/**
* 目標角度設定コマンドのパラメータ、角度データを追加する。
* @method SetServoAngle.addAngle
* @param {int} sid　サーボID 指定範囲 1~254
* @param {float} angle 角度 指定範囲 -180.0~180.0
* @example
* <pre><code>
* var angle =  new vsido.SetServoAngle(15);
* angle.addAngle(2,100.5);
* </code></pre>
**/
vsido.SetServoAngle.prototype.addAngle =  function(sid,angle){
	var data = {};
	if('number' == typeof sid && sid >= 1 && sid <= 254){
		data['sid'] = sid;
	}else{
		return;
	}
	if('number' == typeof angle && angle >= -180.0 && angle <= 180.0){
		data['angle'] = angle;
	}else{
		return;
	}
	this.servo.push(data);
};

/**
* 現在角度取得コマンドを生成する。
* @class GetServoAngle
* @constructor
* @example
* <pre><code>
* var angle = new vsido.GetServoAngle();
* </code></pre>
**/
vsido.GetServoAngle = function() {
	this.command = 'GetServoAngle';
	this.servo = new Array(); /* sid(1~254) */
}
/**
* 現在角度取得コマンドのパラメータ、サーボIDを追加する。
* @method GetServoAngle.addSID
* @param {int} sid　サーボID 指定範囲 1~254
* @example
* <pre><code>
* var angle =  new vsido.GetServoAngle();
* angle.addSID(2);
* </code></pre>
**/
vsido.GetServoAngle.prototype.addSID =  function(sid){
	if('number' == typeof sid && sid >= 1 && sid <= 254){
		this.servo.push(sid);
	}
};



/**
* コンプライアンス設定コマンドを生成する。
* @class SetServoCompliance
* @constructor
* @example
* <pre><code>
* var comp = new vsido.SetServoCompliance();
* </code></pre>
**/
vsido.SetServoCompliance = function() {
	this.command = 'SetServoCompliance';
	this.servo = new Array();
}
/**
* コンプライアンス設定コマンドのパラメータを追加する。
* @method SetServoCompliance.addComp
* @param {int} sid　サーボID 指定範囲 1~254
* @param {int} cpCW 時計回りのコンプライアンススロープ値 指定範囲 1~254
* @param {int} cpCCW 反時計回りのコンプライアンススロープ値 指定範囲 1~254
* @example
* <pre><code>
* var comp = new vsido.SetServoCompliance();
* comp.addComp(2,35,21);
* </code></pre>
**/
vsido.SetServoCompliance.prototype.addComp = function(sid,cpCW,cpCCW) {
	var data = {};
	if('number' == typeof sid && sid >= 1 && sid <= 254){
		data['sid'] = sid;
	}else{
		return;
	}
	if('number' == typeof cpCW && cpCW >= 1 && cpCW <= 254){
		data['cpCW'] = cpCW;
	}else{
		return;
	}
	if('number' == typeof cpCCW && cpCCW >= 1 && cpCCW <= 254){
		data['cpCCW'] = cpCCW;
	}else{
		return;
	}
	this.servo.push(data);
}



/**
* サーボ最小最大可動範囲設定コマンドを生成する。
* @class SetServoMinMaxAngle
* @constructor
* @example
* <pre><code>
* var minMax = new vsido.SetServoMinMaxAngle();
* </code></pre>
**/
vsido.SetServoMinMaxAngle = function() {
	this.command = 'SetServoMinMaxAngle';
	this.servo = new Array();
}
/**
* サーボ最小最大可動範囲設定コマンドのパラメータをセットする。
* @method SetServoMinMaxAngle.addMinMax
* @param {int} sid サーボID 指定範囲　1~254 
* @param {float} min サーボ最小可動範囲　指定範囲　-180.0~180.0
* @param {float} max サーボ最大可動範囲　指定範囲　-180.0~180.0
* @example
* <pre><code>
* var minMax = new vsido.SetServoMinMaxAngle();
* minMax.addMinMax(2,-100,100);
* </code></pre>
**/
vsido.SetServoMinMaxAngle.prototype.addMinMax = function(sid,min,max) {
	var data = {};
	if('number' == typeof sid && sid >= 1 && sid <= 254){
		data['sid'] = sid;
	}else{
		return;
	}
	if('number' == typeof min && min >= -180.0 && min <= 180.0){
		data['min'] = min;
	}else{
		return;
	}
	if('number' == typeof max && max >= -180.0 && max <= 180.0){
		data['max'] = max;
	}else{
		return;
	}
	this.servo.push(data);
}

/**
* サーボ情報読み込みコマンドを生成する。
* @class GetServoInfo
* @constructor
* @example
* <pre><code>
* var info = new vsido.GetServoInfo();
* </code></pre>
**/
vsido.GetServoInfo = function() {
	this.command = 'GetServoInfo';
	this.servo = new Array();
	this.item = new Array();
}
/**
* サーボ情報読み込みコマンドのパラメータ、サーボIDを追加する。
* 一回の送信、最大３個までが追加できる。
* @method GetServoInfo.addSID
* @param {int} sid　サーボID 指定範囲 1~254
* @example
* <pre><code>
* var info =  new vsido.GetServoInfo();
* info.addSID(2);
* </code></pre>
**/
vsido.GetServoInfo.prototype.addSID =  function(sid){
	if(this.servo.length > 3) {
		return;
	}
	if('number' == typeof sid && sid >= 1 && sid <= 254){
		this.servo.push(sid);
	}
};
/**
* サーボ情報読み込みコマンドのパラメータ、項目名を追加する。
* @method GetServoInfo.addItem
* @param {int} item　項目 vsido.SERVO_INFO_ITEM を参照
* @example
* <pre><code>
* var info =  new vsido.GetServoInfo();
* info.addItem('all');
* </code></pre>
**/
vsido.GetServoInfo.prototype.addItem = function(item){
	if('string' == typeof item && -1 != vsido.SERVO_INFO_ITEM.indexOf(item)){
		this.item.push(item);
	}
}


	
/**
* フィードバックID設定コマンドを生成する。
* @class SetFeedbackID
* @constructor
* @example
* <pre><code>
* var fbID = new vsido.SetFeedbackID();
* </code></pre>
**/
vsido.SetFeedbackID = function() {
	this.command = 'SetFeedbackID';
	this.servo = new Array(); /* sid(1~254) */
}
/**
* フィードバックID設定コマンドのパラメータ、サーボIDを追加する。
* @method SetFeedbackID.addSID
* @param {int} sid　サーボID 指定範囲 1~254
* @example
* <pre><code>
* var fbID =  new vsido.SetFeedbackID();
* fbID.addSID(2);
* </code></pre>
**/
vsido.SetFeedbackID.prototype.addSID =  function(sid){
	if('number' == typeof sid && sid >= 1 && sid <= 254){
		this.servo.push(sid);
	}
};



/**
* フィードバック取得コマンドを生成する。
* @class GetServoFeedback
* @constructor
* @example
* <pre><code>
* var feedback = new vsido.GetServoFeedback();
* </code></pre>
**/
vsido.GetServoFeedback = function() {
	this.command = 'GetServoFeedback';
	this.item = new Array();
}
/**
* サーボ情報読み込みコマンドのパラメータ、項目を追加する。
* @method GetServoFeedback.addItem
* @param {String} item　項目 vsido.SERVO_INFO_ITEM を参照
* @example
* <pre><code>
* var feedback =  new vsido.GetServoFeedback();
* feedback.addItem('port_type');
* </code></pre>
**/
vsido.GetServoFeedback.prototype.addItem =  function(item){
	if('string' == typeof item && -1 != vsido.SERVO_INFO_ITEM.indexOf(item)){
		this.item.push(item);
	}
};



/**
* VID設定コマンドを生成する。
* @class SetVIDValue
* @constructor
* @example
* <pre><code>
* var vid = new vsido.SetVIDValue();
* </code></pre>
**/
vsido.SetVIDValue = function() {
	this.command = 'SetVIDValue';
	this.vid = new Array();
}
/**
* VID設定コマンドのパラメータ、項目を追加する。
* @method SetVIDValue.addValue
* @param {String} vid　VID名 vsido.VID_ITEM を参照
* @param {String or int} value　VID値 vsido.VID_ITEM を参照
* @example
* <pre><code>
* var vid =  new vsido.SetVIDValue();
* vid.addValue('RS485_Baudrate','B_115200');
* </code></pre>
**/
vsido.SetVIDValue.prototype.addValue =  function(vid,value){
	if('string' == typeof vid && -1 != vsido.VID_ITEM.indexOf(vid)){
		var data = {};
		if('string' == typeof value && -1 != vsido.VID_VALUE.indexOf(value)){
			data[vid] = value;
			this.vid.push(data);
		}
		if('number' == typeof value ){
			data[vid] = value;
			this.vid.push(data);
		}
	}
};


/**
* VID取得コマンドを生成する。
* @class GetVIDValue
* @constructor
* @example
* <pre><code>
* var vid = new vsido.GetVIDValue();
* </code></pre>
**/
vsido.GetVIDValue = function() {
	this.command = 'GetVIDValue';
	this.vid = new Array();
}
/**
* VID取得コマンドのパラメータ、項目を追加する。
* @method GetVIDValue.addVID
* @param {String} item　項目名 vsido.VID_ITEM を参照
* @example
* <pre><code>
* var vid =  new vsido.GetVIDValue();
* vid.addVID('RS485_Baudrate');
* </code></pre>
**/
vsido.GetVIDValue.prototype.addVID =  function(vid){
	if('string' == typeof vid && -1 != vsido.VID_ITEM.indexOf(vid)){
		this.vid.push(vid);
	}
};



/**
* フラッシュ書き込みコマンドを生成する。
* @class WriteFlash
* @constructor
* @example
* <pre><code>
* var flash = new vsido.WriteFlash();
* </code></pre>
**/
vsido.WriteFlash = function() {
	this.command = 'WriteFlash';
}

/**
* IO設定コマンドを生成する。
* @class SetGPIOValue
* @constructor
* @example
* <pre><code>
* var gpio = new vsido.SetGPIOValue();
* </code></pre>
**/
vsido.SetGPIOValue = function() {
	this.command = 'SetGPIOValue';
	this.gpio = new Array();
}
/**
* IO設定コマンドのパラメータをセットする。
* @method SetGPIOValue.setValue
* @param {int} port ポート番号 指定範囲 4~7
* @param {int} val 0~1 
* @example
* <pre><code>
* var gpio = new vsido.SetGPIOValue();
* gpio.setValue(4,0);
* </code></pre>
**/
vsido.SetGPIOValue.prototype.setValue = function(port,val) {
	var data = {};
	if('number' == typeof port && port >= 4 && port <= 7){
		data['port'] = port;
	}else{
		return;
	}
	if('number' == typeof val && val >= 0 && val <= 1){
		data['val'] = val;
	}else{
		return;
	}
	this.gpio.push(data);
}

/**
* PWM設定コマンドを生成する。
* @class SetPWMPulse
* @constructor
* @example
* <pre><code>
* var pwm = vsido.SetPWMPulse();
* </code></pre>
**/
vsido.SetPWMPulse = function() {
	this.command = 'SetPWMPulse';
	this.pwm = new Array();
}
/**
* PWM設定コマンドのパラメータを生成する。
* @method SetPWMPulse.setWidth
* @param {int} port ポート番号 指定範囲 6~7
* @param {int} width パルス幅 指定範囲 0~65532 単位us 精度4us
* @example
* <pre><code>
* var pwm = vsido.SetPWMPulse();
* pwm.setWidth(6,1523);
* </code></pre>
**/
vsido.SetPWMPulse.prototype.setWidth = function(port,width) {
	var data = {};
	if('number' == typeof port && port >= 6 && port <= 7){
		data['port'] = port;
	}else{
		return;
	}
	if('number' == typeof width && width >= 0 && width <= 65535){
		data['width'] = width;
	}else{
		return;
	}
	this.pwm.push(data);
}
/**
* PWM設定コマンドのパラメータを生成する。
* @method SetPWMPulse.setDuty
* @param {int} port ポート番号 指定範囲 6~7
* @param {float} duty パルスDuty 指定範囲　0.0~1.0
* @example
* <pre><code>
* var pwm = vsido.SetPWMPulse();
* pwm.setDuty(6,0.25);
* </code></pre>
**/
vsido.SetPWMPulse.prototype.setDuty = function(port,duty) {
	var data = {};
	if('number' == typeof port && port >= 6 && port <= 7){
		data['port'] = port;
	}else{
		return;
	}
	if('number' == typeof duty && duty >= 0.0 && duty <= 1.0){
		data['duty'] = duty;
	}else{
		return;
	}
	this.pwm.push(data);
}



/**
* 接続確認コマンドを生成する。
* @class CheckConnectedServo
* @constructor
* @example
* <pre><code>
* var connectedInfo = new vsido.CheckConnectedServo();
* </code></pre>
**/
vsido.CheckConnectedServo = function() {
	this.command = 'CheckConnectedServo';
}


/**
* IK設定コマンドを生成する。
* @class SetIK
* @constructor
* @param {object} ikflag　IK設定フラグ
* <pre><code>
* {'position':boolean,'rotation':boolean,'torque':boolean}
* </code></pre>
* @example
* <pre><code>
* var ik = new vsido.SetIK({'position':true});
* </code></pre>
**/
vsido.SetIK = function (ikflag){
	this.command='SetIK';
	if(ikflag){
		this.ikflag = ikflag;
	}else {
		this.ikflag = {
			'position':false,'rotation':false,'torque':false
		};
	}
	this.kdt=new Array();
};
/*
	private function.
*/
vsido.SetIK.prototype.addKDTbyTag = function(kid,tag,x,y,z){
	var data = {};
	var dataIndex= -1;
	if(kid){
		for(var i = 0; i < this.kdt.length; i++){
			if(this.kdt[i]&& kid == this.kdt[i]['kid']){
				dataIndex = i;
				data = this.kdt[i];
				break;
			}
		}
		if(-1 == dataIndex) {
			if( 'string' == typeof kid && -1 != vsido.KID_ITEM.indexOf(kid)){
				data['kid'] = kid;
			}else{
				if('number' == typeof kid && kid >= 0 && kid <= 15){
					data['kid'] = kid;
				}else {
					return;
				}
			}
		}
	}
	else {
		return;
	}
	
	data[tag] = {}
	if('number' == typeof x && x >= -100 && x <= 100){
		data[tag]['x'] = x;
	}
	else {
		return;
	}
	if('number' == typeof y && y >= -100 && y <= 100){
		data[tag]['y'] = y;
	}
	else {
		return;
	}
	if('number' == typeof z && z >= -100 && z <= 100){
		data[tag]['z'] = z;
	}
	else {
		return;
	}
	if(-1 != dataIndex) {
		this.kdt.splice(dataIndex,1);
	}
	this.kdt.push(data);
};

/**
* IK設定コマンドのパラメータ、位置をセットする。
* @method SetIK.setPosition
* @param {String int} kid　指定範囲 vsido.KID_ITEM を参照  or 0-15
* @param {int} x  位置X軸 -100~100　単位(%)
* @param {int} y  位置Y軸 -100~100　単位(%)
* @param {int} z  位置Z軸 -100~100　単位(%)
* @example
* <pre><code>
* var ik = new vsido.SetIK({'position':true});
* ik.setPosition(2,10,-10,20);
* </code></pre>
**/
vsido.SetIK.prototype.setPosition = function(kid,x,y,z) {
	var data = this.addKDTbyTag(kid,'position',x,y,z);
}

/**
* IK設定コマンドのパラメータ、姿勢をセットする。
* @method SetIK.setRotation
* @param {String int} kid　指定範囲 vsido.KID_ITEM を参照  or 0-15
* @param {int} x 姿勢X軸 -100~100 単位(%)
* @param {int} y 姿勢Y軸 -100~100　単位(%)
* @param {int} z 姿勢Z軸 -100~100　単位(%)
* @example
* <pre><code>
* var ik = new vsido.SetIK({'rotation':true});
* ik.setRotation(2,10,-10,20);
* </code></pre>
**/
vsido.SetIK.prototype.setRotation = function(kid,x,y,z) {
	var data = this.addKDTbyTag(kid,'rotation',x,y,z);
}

/**
* IK設定コマンドのパラメータ、トルクをセットする。
* @method SetIK.setTorque
* @param {String int} kid　指定範囲 vsido.KID_ITEM を参照  or 0-15
* @param {int} x トルクX軸 -100~100　単位(%)
* @param {int} y トルクY軸 -100~100　単位(%)
* @param {int} z トルクZ軸 -100~100　単位(%)
* @example
* <pre><code>
* var ik = new vsido.SetIK({'torque':true});
* ik.setTorque(2,10,-10,20);
* </code></pre>
**/
vsido.SetIK.prototype.setTorque = function(kid,x,y,z) {
	var data = this.addKDTbyTag(kid,'torque',x,y,z);
}

/**
* IK読み込みコマンドを生成する。 
* @class GetIK
* @constructor
* @param {object} ikflag　IK設定フラグ 
* <pre><code>
* {'position':boolean,'rotation':boolean,'torque':boolean};
* </code></pre>
* @example
* <pre><code>
* var ik = new vsido.GetIK({'position':true});
* </code></pre>
**/
vsido.GetIK = function(ikflag) {
	this.command = "GetIK";
	if(ikflag){
		this.ikflag = ikflag;
	}else {
		this.ikflag = {
			'position':false,'rotation':false,'torque':false
		};
	}
	this.kid = new Array();
}
/**
* IK読み込みコマンドのパラメータ、KIDを追加する。
* @method GetIK.addKID
* @param {String int} kid　指定範囲 vsido.KID_ITEM を参照  or 0-15
* @example
* <pre><code>
* var ik =  new vsido.GetIK({'position':true});
* ik.addKID(2);
* </code></pre>
**/
vsido.GetIK.prototype.addKID =  function(kid){
	if('string' == typeof kid && -1 != vsido.KID_ITEM.indexOf(kid)){
		this.kid.push(kid);
		return;
	}
	if('number' == typeof kid && kid >= 0 && kid <= 15){
		this.kid.push(kid);
	}
};


/**
* 歩行設定コマンドを生成する。 
* @class Walk
* @constructor
* @param {int} forward　前後の速度 指定範囲 -100~100 　単位(%)　＋前進　-後退
* @param {int} turnCW　旋回の速度 指定範囲 -100~100 　単位(%)　＋時計回り　－反時計回り
* @example
* <pre><code>
* var walk = new vsido.Walk(50,0);
* </code></pre>
**/
vsido.Walk = function(forward,turnCW) {
	this.command = 'Walk';
	if('number' == typeof forward && forward >= -100  && forward <= 100) {
		this.forward= forward;
	}else{
		this.forward= 0;
	}
	if('number' == typeof turnCW && turnCW >= -100  && turnCW <= 100) {
		this.turnCW= turnCW;
	}else{
		this.turnCW= 0;
	}
}
	
/**
* 加速度センサ値取得コマンドを生成する。
* @class GetAcceleration
* @constructor
* @example
* <pre><code>
* var aSensor = new vsido.GetAcceleration();
* </code></pre>
**/
vsido.GetAcceleration = function() {
	this.command = 'GetAcceleration';
}

/**
* 電源電圧取得コマンドを生成する。
* @class GetVoltage
* @constructor
* @example
* <pre><code>
* var volt = new vsido.GetVoltage();
* </code></pre>
**/
vsido.GetVoltage = function() {
	this.command = 'GetVoltage';
}

	
/**
* 任意バイナリコマンドを生成する。
* @class Binary
* @constructor
* @param {Byte} op　コマンド 指定範囲 0~127
* @example
* <pre><code>
* var binary = new vsido.Binary(0x63);
* binary['bin'].push([0x0,0x11]);
* </code></pre>
**/
vsido.Binary = function(op) {
	this.command = 'Binary';
	if('number' == typeof op) {
		this.op = op;
	} else {
		this.op = 0;
	}
	this.bin = new Array();
}

/**
* RAW コマンド(pass through)を生成する。（ST、サイズ付き、チェックSUM付き）
* RAW コマンドはV-Sido CONNECTにそのまま送られる。
* @class execRaw
* @constructor
* @example
* <pre><code>
* var raw = new vsido.execRaw();
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
vsido.execRaw = function() {
	this.command = 'Raw',
	this.exec = new Array();
}
