ws = new WebSocket('ws://192.168.0.2/board');//à adapter suivant l'addresse du serveur
var arr=new Array(4);
$(document).ready(function(){
	var sel=4;
	 //var xml="<capteurs><lum><state>1</state><val>10</val><val>11</val></lum><can><state>1</state><val>134</val><val>98</val></can><acc><state>1</state><val>134</val><val>98</val><val>98</val></acc><temp><state>1</state><val>20</val><val>98</val></temp></capteurs>";
	 //var xml2="<capteurs><lum><state>1</state><val>19</val><val>11</val></lum><can><state>1</state><val>1387</val><val>98</val></can><acc><state>1</state><val>13</val><val>91</val><val>9</val></acc><temp><state>1</state><val>29</val><val>98</val></temp></capteurs>";
	function init () {
	        ws.onopen = function(e) {
	            //ws.send(xml);
	            //setTimeout(function(){  ws.send(xml2); }, 9000);
	        };
	        ws.onmessage = function(msg) {
	        	if(msg.data!=0 && msg.data!=1 && msg.data!=2){
		        	var xmlDoc = $.parseXML( msg.data );
		 			$xml=$(xmlDoc);
				 	 arr[0]=new Array(4);
						 arr[0][0]="lum";
						 arr[0][1]=$xml.find("lum").find("state").text();
						 arr[0][2]=$xml.find("lum").find("val").first().text();
						 arr[0][3]=$xml.find("lum").find("val").first().next().text();
					arr[1]=new Array(4);
						arr[1][0]="can";
						arr[1][1]=$xml.find("can").find("state").text();
						arr[1][2]=$xml.find("can").find("val").first().text();
						arr[1][3]=$xml.find("can").find("val").first().next().text();
					arr[2]=new Array(5);
						arr[2][0]="acc";
						arr[2][1]=$xml.find("acc").find("state").text();
						arr[2][2]=$xml.find("acc").find("val").first().text();
						arr[2][3]=$xml.find("acc").find("val").first().next().text();
						arr[2][4]=$xml.find("acc").find("val").first().next().next().text();
					arr[3]=new Array(4);
						arr[3][0]="temp";
						arr[3][1]=$xml.find("temp").find("state").text();
						arr[3][2]=$xml.find("temp").find("val").first().text();
						arr[3][3]=$xml.find("temp").find("val").first().next().text();
					if(sel!=0 && sel!=5)
		        			updateComponent(sel);
	        	}
	     	};
	}
	init();
	function updateComponent (index) {
			console.log("here");
			switch(index){
				case 1:
					sel=1;
					$("#acc-content").show().promise().done(function () {
							var html='<div id="acc"><img height="200" width="200" src="./img/acc.png" alt="acc"></div>';
						    $(this).html(html); 
						    $(this).show().promise().done(function() {
						    	$("#acc").show();
						    });
						    $(this).append('<P class="lead">x: ' + arr[2][2] + ' <br />y: ' +arr[2][3] + '<br /> z: ' + arr[2][4] + '</p>');
						});
					break;
				case 2:
					sel=2;
						$("#temp-content").show().promise().done(function () {
						var html='<div id="temp"><img height="450" width="450" src="./img/Temp.jpg" alt="temp"></div>';
					    $(this).html(html); 
					    $(this).show().promise().done(function() {
					    	$("#temp").fadeTo("slow",1);
					    });
					    $(this).append('<P class="lead">' + arr[3][2] + ',' + arr[3][3] +"°C");
					});
					break;
				case 3:
					sel=3;
					$("#lum-content").show().promise().done(function () {
						var html='<div id="bulb"><img height="200" width="200" src="./img/bulb.jpg" alt="bulb"></div>';
					    $(this).html(html); 
					    $(this).show().promise().done(function() {
					    	$("#bulb").fadeTo("slow",arr[0][2]/10);
					    });
					    $(this).append('<P class="lead">'+arr[0][2]+","+arr[0][3]+' Lux</p>');
					});
					break;
				case 4:
					sel=4;
					$("#adc-content").show().promise().done(function () {
						var html='<div id="adc"><img height="450" width="450" src="./img/ADC.jpg" alt="adc"></div>';
					    $(this).html(html); 
					    $(this).show().promise().done(function() {
					    	$("#adc").fadeTo("slow",1);
					    });
					    $(this).append('<P class="lead">' + arr[1][2] + ' et ' + arr[1][3] + '</p>');
					});
					break;
			}			
	}
	$(".mylist li").click(function(event){
		event.preventDefault();
		$(".mylist li").removeClass("active");
		$(this).addClass("active");
		updateComponent(parseInt($(this).attr("data-selection")));
	});

});
