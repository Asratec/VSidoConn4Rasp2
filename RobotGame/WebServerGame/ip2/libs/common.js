var IP_TYPE={
	NONE:0,
	VALID:1,
	INVALID:2,
};
function validateIPtype(){
	for(var i=1;i<=4;i++){
		var input=$('#ip-'+i).val();
		if(!input) return IP_TYPE.NONE;
		if(input=="") return IP_TYPE.INVALID;
		if(i==1&&input==0) return IP_TYPE.INVALID;
	}
	return IP_TYPE.VALID;
}
function getIP(){
	var ip="";
	for(var i=1;i<=4;i++){
		if(i!=1) ip+=".";
		ip+=$('#ip-'+i).val();
	}
	return ip;
}
function getUrlVars(){
	var vars=[], hash;
	var hashes=window.location.href.slice(window.location.href.indexOf('?')+1).split('&');
	for(var i= 0;i<hashes.length;i++)
	{
		hash=hashes[i].split('=');
		vars.push(hash[0]);
		vars[hash[0]]=hash[1];
	}
	return vars;
}
function initIPinput(){
	var ip=getUrlVars()["setip"];
	if(ip){
		var ipparts=ip.split('.');
		for(var i=1;i<=ipparts.length;i++){
			var obj=$('#ip-'+i);
			if(obj){
				obj.val(ipparts[i-1]);
			}
		}
	}
}
function callback(){
	var components=$('#ip-1,#ip-2,#ip-3,#ip-4');
	if(components.hasClass('error')){
		components.removeClass('error');
	}
}
$(function(){

	$('#ipset')
	.button()
	.on('click',function(){
		var currurl=document.URL;
		currurl=currurl.split('?')[0];
		var valid=validateIPtype();
		if(valid==IP_TYPE.VALID){
			currurl+="?setip="+getIP();
			document.location.href=currurl;
		}else{
			var components=$('#ip-1,#ip-2,#ip-3,#ip-4');
			if(!components.hasClass('error')){
				components.addClass('error');
			}
			$('#ip-1,#ip-2,#ip-3,#ip-4').effect('highlight',{},1000,callback);
		}
	});

	$(window).load(function(){
		initIPinput();
	});
	var s_pagetbl={
		"0":"./index.html",
		"1":"./robot.html",
		"2":"./target.html",
		"3":"./ik.html",
		"4":"./movement.html",
		"5":"./command.html",
		"6":"./camera.html",
		"7":"./vr.html",
	};
	// mainbtn‘€ì
	$(".mainbtn").on("click",function(){
		var btnobj=$(this);
		var btnid=btnobj.data("id");
		var url=s_pagetbl[btnid+""];
		var valid=validateIPtype();
		if(valid==IP_TYPE.VALID){
			url+="?setip="+getIP();
		}
		document.location.href=url;
	});

	/*
	// tooltip‰Šú‰»
	$(document).tooltip({
		position:{
			my:"center top",
			at:"center bottom",
		},
			show:{
			duration:"fast"
		},
			hide:{
			effect:"hide"
		}
	});
	*/
	
	// ipƒAƒhƒŒƒX“ü—Í
	function checkIPrange(input){
		var num=input;
		if(!$.isNumeric(num)){
			num=0;
		}else{
			if(num<0){
				num=0;
			}else if(num>255){
				num=255;
			}
		}
		return num;
	}
	$('#ip-1,#ip-2,#ip-3,#ip-4')
	.attr('maxlength',3)
	.keypress(function(event,ui){
		return (event.charCode>=48 && event.charCode<=57);
	})
	.focusout(function(event,ui){
		var num=$(this).val();
		num=checkIPrange(num);
		$(this).val(num);
	});
	
});