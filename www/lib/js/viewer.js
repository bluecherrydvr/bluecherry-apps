window.addEvent('load', function(){
	var layoutsMenu = new ContextMenu({
		menu:	'layoutsMenu',
		targets: '#layoutsControl',
		trigger: 'click',
		actions: {
			'save' : function(){
				layoutsUpdate('edit', Cookie.read('currentLayout'));
			},
			'saveAs' : function(){
				var layoutName = prompt('New layout name:');
				if (layoutName != null){
					layoutsUpdate('new', layoutName);
				}
			},
			'delete' : function(){
				layoutsUpdate('delete', Cookie.read('currentLayout'));
				Cookie.write('currentLayout', '');
			},
			'load' : function(el, ref, item){
				layoutsUpdate('load', item.get('html'));
				Cookie.write('currentLayout', item.get('html'));
			},
			'clearAll' : function(){
				layoutsUpdate('clear', false);
				Cookie.write('currentLayout', '');
			}
		}
	});
	makeGrid();
	adjustImageSize();
	var cameraMenu = new ContextMenu({
		menu:	'cameraList',
		targets: '.noImg',
		trigger: 'click',
		actions: {
			'loadCam' : function(el, ref, item){
				var device_id = item.get('id');
				var elParent = el.getParent();
				var videoMethod = $$('#video_method').get('value')[0];

				Cookie.write('imgSrc'+elParent.getParent().get('class')+elParent.get('class'), device_id);

				if (videoMethod === 'HLS') {
					el.set('src', '/img/icons/layouts/loading.png');
					createHlsLink(device_id, el.get('id'), 'noImg', window.location.hostname, window.location.port);
				}
				else {
					el.set('src', '/media/mjpeg?multipart=true&id='+device_id);
				}

				if (item.get('class')){
					addPtz(el, device_id);
				};
			}
		}
	});
	$$('.ac').addEvent('click', function(){ addDelRowColumn('c', true); });
	$$('.dc').addEvent('click', function(){ addDelRowColumn('c', false); });
	$$('.ar').addEvent('click', function(){ addDelRowColumn('r', true); });
	$$('.dr').addEvent('click', function(){ addDelRowColumn('r', false); });
	
	$$('.l1').addEvent('click', function(){ setLayout(1);	});
	$$('.l4').addEvent('click', function(){ setLayout(4);	});
	$$('.l9').addEvent('click', function(){ setLayout(9);	});
	$$('.l16').addEvent('click', function(){ setLayout(16); });
	$('logout').addEvent('click', function(){
		document.location = '/logout';
	});
	$('profilePage').addEvent('click', function(){
		document.location = '/profile';
	});
	$('playbackPage').addEvent('click', function(){
		document.location = '/playback';
	});
	$('backToAdmin').addEvent('click', function(){
		document.location = '/';
	});
	$$('.presets').each(function(el){
		presetmenuvar = new ContextMenu({
					menu: 'presets-'+el.getParent().get('id'),
					targets: '#'+el.get('id'),
					trigger: 'click',
					actions:{
						'goto': function(el, ref, item){
							presetRequest('go', item.get('id'), item.get('name'));
						},
						'rename': function(el, ref, item){
							var presetName = prompt('New preset name:');
							if (presetName != null){
								presetRequest('rename', item.get('id'), item.get('name'), presetName);
							}
						},
						'delete': function(el, ref, item){
							presetRequest('clear', item.get('id'), item.get('name'));
						},
						'map': function(el, ref, item){
							var presetName = prompt('New preset name:');
							if (presetName != null){
								presetRequest('save', item.get('id'), item.get('name'), presetName);
							}							
						}
					}
		});
	});
});

//functions/classes

presetRequest = function(command, presetId, cameraId, name){
	var data = 'id='+cameraId+'&command='+command;
	if (command=='rename' || command=='save'){
		data += '&name='+name;
	};
	if (command!='save'){
		data += '&preset='+presetId;
	}
	var request = new Request.HTML({
		url: 'media/ptz',
		data: data,
		method: 'get',
		onRequest: function(){
		},
		onComplete: function(){
		},
		onFailure: function(){
		},
		onSuccess: function(tree, elements, html){
		}
	}).send();
}

addPtz = function(el, id){
	var ptzTable = new Element('div', {'class': 'ptzControls', 'id' : id, 'html' : '<div class="lu"></div><div class="nu"></div><div class="ru"></div><div class="ln"></div><div class=""></div><div class="rn"></div><div class="ld"></div><div class="nd"></div><div class="rd"></div><div class="t">+</div><div class="w">&ndash;</div><div class="presets" id="presetButton">p</div>'});
	ptzTable.inject(el.getParent(), 'bottom');
	ptzTable.getChildren().each(function(el){
		if (el.get('class')!='presets'){
			el.addEvents({
				'mousedown': function(ev){
					ev.stopPropagation();
					sendPtzCommand(this.getParent().get('id'), 'move', el.get('class'), true);
				},
				'mouseup': function(){
					var cmd = (el.get('class')=='t' || el.get('class') == 'w') ? 'stop_zoom' : 'stop'; 				
					sendPtzCommand(this.getParent().get('id'), cmd, el.get('class'), true);
				},
				'mouseout': function(){
					var cmd = (el.get('class')=='t' || el.get('class') == 'w') ? 'stop_zoom' : 'stop';
					sendPtzCommand(this.getParent().get('id'), cmd, el.get('class'), true);
				}
			});
		} else if ($('presets-'+id)==null) {
			el.setStyle('display', 'none');		
		}
	});
	
	
}

layoutsUpdate = function(mode, layout){
	var request = new Request({
		url: 'liveview/layouts',
		data: 'mode='+mode+'&layout='+layout,
		method: 'post',
		onRequest: function(){
		},
		onSuccess: function(text, xml){
			window.location.reload(true);
		},
		onFailure: function(){
		}
	}).send();
}

makeGrid = function(){
	
	var lvRows = (Cookie.read('lvRows') || 2);
	var lvCols = (Cookie.read('lvCols') || 2);
	var gridTable = new Element('table', {
           	'id' : 'lvGridTable', 'class': 'webcamsTable'
    });
	for (row = 1; row <= lvRows; row++) {
       	var thisRow = new Element('tr', {'id' : row,'class' : 'y'+row});
       	for(col = 1; col <= lvCols; col++){
			var thisCol = new Element('td', {'id' : col, 'class' : 'x'+col});
			var imgSrcId = (Cookie.read('imgSrcy'+row+'x'+col) || 'none');
			var imgClass = 'noImg gridImage';
			var thisCam = $$('.ptz'+'#'+imgSrcId);
			var id = imgSrcId;
			var videoMethod = $$('#video_method').get('value')[0];
			var elementId = 'live_view' + row + col;

			if (videoMethod === 'HLS') {
				imgSrcId = (imgSrcId!='none') ? '/img/icons/layouts/loading.png' : '/img/icons/layouts/none.png';
			}
			else {
				imgSrcId = (imgSrcId!='none') ? '/media/mjpeg?multipart=true&id='+imgSrcId : '/img/icons/layouts/none.png';
			}
			
			var lvImg = new Element('img', {'class': imgClass, 'src': imgSrcId, 'id': elementId});
			lvImg.inject(thisCol);

			if (thisCam  && (thisCam.get('id') == id)) {
				addPtz(lvImg, id);
			}

            thisCol.inject(thisRow, 'bottom');

			if (videoMethod === 'HLS' && id != 'none') {
				createHlsLink(id, elementId, imgClass, window.location.hostname, window.location.port);
			}
		};
  	    thisRow.inject(gridTable, 'bottom');
	};
    gridTable.inject($('liveViewContainer'));
}

adjustImageSize = function(){
	//based on 704/480 aspect for ntsc

	var lvRows = (Cookie.read('lvRows') || 2);
	var lvCols = (Cookie.read('lvCols') || 2);
	var verticalAdj = 60;
	var horizontalAdj = 250;
	var maxHeight = 0;
	var maxWidth = 0;  
	if ((window.innerHeight-verticalAdj)/(lvRows*480) < (window.innerWidth-horizontalAdj)/(lvCols*704)){
		maxHeight = (window.innerHeight-verticalAdj)/(lvRows);
		maxWidth = 704*(maxHeight/480);
	} else {
		maxWidth = (window.innerWidth-horizontalAdj)/(lvCols);
		maxHeight = 480*(maxWidth/704);
	}
	
	$$('#liveViewContainer table tr td img').each(function(el){
		if ((el.width/el.height) > (704/480)){
			el.setStyle('width', maxWidth);
			el.setStyle('height', '');
			el.setStyle('min-height', maxHeight);
		}
		else {
			el.setStyle('height', maxHeight);
			el.setStyle('width', '');
			el.setStyle('min-width', maxWidth);
		}
	});
	
	$$('#liveViewContainer table tr td video').each(function(el){
		if ((el.width/el.height) > (704/480)){
			el.setStyle('width', maxWidth);
			el.setStyle('height', '');
			el.setStyle('min-height', maxHeight);
		}
		else {
			el.setStyle('height', maxHeight);
			el.setStyle('width', '');
			el.setStyle('min-width', maxWidth);
		}
	});

	window.addEvent('resize', function(){
		adjustImageSize();
	});
}

addDelRowColumn = function(n, t){
	var tp = ((n)=='c' ? 'lvCols' : 'lvRows');
	var v = parseInt(Cookie.read(tp) || 2);
	Cookie.write(tp, ((t) ? v+1 : v-1));
	window.location.reload(true);
},
	
setLayout = function(tp){
	Cookie.write('lvCols', Math.sqrt(tp)); Cookie.write('lvRows', Math.sqrt(tp));
	window.location.reload(true);
}

sendPtzCommand = function(camId, command, d, cont, speed){
	if (!speed) var speed = 32;
	var data = 'id='+camId+'&panspeed='+speed+'&tiltspeed='+speed;
	data += '&command='+command;
	if (command != "stop"){		
		data += (cont) ? '&duration=-1' : '&duration=250';
		if (d!='t' && d!='w'){
			if (d.substring(0,1)!='n') { data += '&pan='+d.substring(0,1); };
			if (d.substring(1,2)!='n') { data += '&tilt='+d.substring(1,2); };
		} else {
			data += '&zoom='+d;
		}
	}
	var request = new Request.HTML({
		url: 'media/ptz',
		data: data,
		method: 'get',
		onRequest: function(){
		},
		onComplete: function(){
		},
		onFailure: function(){
		},
		onSuccess: function(tree, elements, html){
		}
	}).send();
};

setHlsLink = function(videoLink, videoId, imgClass) {

	var element = document.getElementById(videoId);
	var parent = element.parentNode;

	var width     = element.style.width;
	var height    = element.style.height;
	var minWidth  = element.style.minWidth;
	var minHeight = element.style.minHeight;

	parent.removeChild(element);

	if(Hls.isSupported()) {
		var hls = new Hls();

		element = new Element('video', {'class': imgClass, 'id': videoId, 'controls': '', 'autoplay': 'true', 'width': '100%', 'height': 'auto'});
		hls.loadSource(videoLink);
		hls.attachMedia(element);
		hls.on(Hls.Events.MANIFEST_PARSED,function() {
			video.play();
		});
	} else if (video.canPlayType('application/vnd.apple.mpegurl')) {
		element = new Element('video', {'class': imgClass, 'id': videoId, 'controls': '', 'autoplay': 'true', 'width': '100%', 'height': 'auto'});
		element.src = videoLink;
		element.addEventListener('loadedmetadata',function() {
			element.play();
		});
	} else {
		element = new Element('img', {'class': imgClass, 'id': videoId, 'src': '/img/icons/layouts/hls-not-support.png'});
	}

	element.style.width     = width;
	element.style.height    = height;
	element.style.minWidth  = minWidth;
	element.style.minHeight = minHeight;
	
	parent.appendChild(element);
}

setHlsErrorImage = function(elementId, message) {

	var element = document.getElementById(elementId);
	if (message === "no_permission") {
		element.src = '/img/icons/layouts/no-permission.png';
	}
	else if (message === "disabled") {
		element.src = '/img/icons/layouts/disabled.png';
	} 
	else if (message === "not_found") {
		element.src = '/img/icons/layouts/not-found.png';
	} 
	else if (message === "not_id") {
		element.src = '/img/icons/layouts/no-device-id.png';
	} 
	else {
		//
	} 
}

createHlsLink = function(deviceId, elementId, elementClass, hostname, port) {
	var data = 'id=' + deviceId + '&hostname=' + hostname + '&port=' + port;
	var request = new Request.HTML({
		url: 'media/hls',
		data: data,
		method: 'get',
		onRequest: function(){
			console.log('onRequest');
		},
		onComplete: function(){
			console.log('onComplete');
		},
		onFailure: function(xhr){
			console.log(xhr);
		},
		onSuccess: function(json){
			var ret = JSON.parse(json[0].data);
			if (ret.status == 6) {
				var hls_link = ret.msg[0];
				setHlsLink(hls_link, elementId, elementClass);
			}
			else {
				setHlsErrorImage(elementId, ret.msg[0]);
			}
		}
	}).send();

}