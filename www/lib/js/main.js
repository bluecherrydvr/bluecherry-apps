DVRmainMenu = new Class({
	initialize: function(){
		$$('#menuButtons li').each(function(el){
			var morph = new Fx.Morph(el,{ 'duration':'200', link:'cancel' });
			var original = (el.getStyle('background-color')=='transparent') ? '#fff' : el.getStyle('background-color') ;
			el.addEvents({
				'mouseenter' : function() { morph.start({ 'background-color':'#ff8c00' }); el.setStyle('cursor', 'pointer');},
				'mouseleave' : function() { morph.start({ 'background-color': original }); el.setStyle('cursor', ''); },
				'click'		 : function(ev) { if (el.get('class')=='liveView') { window.location.href = '/?l=true'; return false; };  ev.preventDefault(); openPage = new DVRPage(this.get('id')); }

			});
		});
	}
});

DVRPage = new Class({
	initialize: function(page, data, status, msg){
		$$('.liveViewImg').set('src', '');
		var request = new Request.HTML({
			url: 'ajax/'+page+'.php',
			data: data,
			evalScripts: true,
			method: 'get',
			onRequest: function(){
				tempOverlay = new DVRContainerOverlay();
			},
			onFailure: function(){
				$(containerDivID).set('html',$('onLoadFailureContainer').get('html')).fade('in');
			},
			onSuccess: function(tree, elements, html){
				$(containerDivID).set('html', html).fade('in');
				pageScript = new DVRPageScript(page);
				if (status) { var showMessage = new DVRMessage(status, msg); };
				Cookie.write('currentPage', page);
				Cookie.write('currentPageData', data);
			}
		}).send();
	}
});

DVRContainerOverlay = new Class({
	initialize: function(){
		overlay = new Element('div', {'id': 'pageContainerLoadingOverlay'});
		overlay.inject($(containerDivID));
	},
	removeoverlay: function(){
		$('pageContainerLoadingOverlay').dispose();
	}
});



DVRPageScript = new Class({
	initialize: function(page){
		switch (page){
			//userspage
			case 'users':
				$$('#user-table tr').each(function(el){
					if (el.get('class')!='header'){
						el.addEvent('click', function() { openPage = new DVRPage('users', 'id='+this.get('id')); });
					};
				});
				if ($('allusers')!=null){
					//back to list button
					$('allusers').addEvent('click', function(){
						openPage = new DVRPage('users');
					});
					//user details form
					var userForm = new DVRSettingsForm('settingsForm');
					$('deleteButton').deleteUserButton();
					
				};
				if ($('newUser')!=null){
					buttonMorph($('newUser'), '#8bb8');
					$('newUser').addEvent('click', function() {
									openPage = new DVRPage('users', 'id=new');
					});
					$$('#user-table .deleteUser').each(function(el){
						el.addEvents({
							'click' : function(ev){
								ev.stopPropagation();
								el.highlight('#faa');
								var userID = this.getParent().get('id');
								deleteUser(userID, 'list');
							},
							'mouseenter' : function(ev){
								ev.stopPropagation();
								el.setStyle('cursor','pointer');
							},
							'mouseleave' : function(ev){
								el.setStyle('cursor','');
							}
						});
					});
					
				};

				
			break;//end users
			case 'log' :
				$('dvrlog').setStyle('height', window.innerHeight-180);
				var scrollLog = new Fx.Scroll('dvrlog').toBottom();
				$('scrollLogToTop').addEvent('click', function(ev){
					ev.preventDefault();
					scrollLog.toTop();
				});
				$('scrollLogToBottom').addEvent('click', function(ev){
					ev.preventDefault();
					scrollLog.toBottom();
				});
				$('logLoader').addEvent('change', function(){
					openPage = new DVRPage('log', 'lines='+this.get('value'));
				});
			break; //end log
			case 'devices' :
				$('addIPCamera').addEvent('click', function(){
					openPage = new DVRPage('addip');
				});
				$('addIPCamera').buttonAnimate("#aca")
				initNames = function(){
					$$('.name').each(function(el){
						el.addEvent('click', function(){
							var id = el.get('id');						
							el.getParent().set('html', '<div class="name"><input type="text" class="cDevName" value="'+el.get('html')+'"></div>');
							$$('.cDevName').each(function(el){
								el.addEvents({
									'change': function(){
										el.getParent().set('html', el.get('value'));
										ajaxUpdateField('update', 'Devices', {'device_name' : el.get('value')}, id, 'none');
										initNames();
									},
									'blur': function(){
										el.getParent().set('html', el.get('value'));
										ajaxUpdateField('update', 'Devices', {'device_name' : el.get('value')}, id, 'none');
										initNames();
									}
								});
							});
						});
					});
				};
				initNames();
				$$('.cardEncoding').each(function(el){
					el.addEvent('click', function(){
						var encoding = (el.get('html')=='NTSC') ? 'PAL' : 'NTSC';
						ajaxUpdateField('updateEncoding', 'Devices', {'signal_type' : encoding }, el.get('id'), 'devices');
					});
				});
				$$('.editMap').each(function(el){
					el.addEvent('click', function(){
						var page = new DVRPage('motionmap', 'id='+el.get('id'));
					});
				});
				$$('.deviceSchedule').each(function(el){
					el.addEvent('click', function(){
						var page = new DVRPage('deviceschedule', 'id='+el.get('id'));
					});
				});
				$$('.change_state').each(function(el){
					el.addEvent('click', function(){
						ajaxUpdateField('changeState', 'Devices', {'do' : true }, el.get('id'), 'devices');
					});
				});
				$$('.videoadj').each(function(el){
					el.addEvent('click', function(){
						var page = new DVRPage('videoadj', 'id='+el.get('id'));
					});
				});
				$$('.RES').each(function(el){
					el.addEvent('change', function(ev){
						upadteResFps(el);
					});
				});
				$$('.FPS').each(function(el){
					el.addEvent('change', function(ev){
						upadteResFps(el);
					});
				});
				var localDevicesTabOpen = (Cookie.read('localDevicesTabOpen') || -1);
				var myAccordion = new Accordion($('devicesGroup'), 'div.cardHeader', 'div.cardContent', { alwaysHide: true, display: localDevicesTabOpen, initialDisplayFx: false,
					onActive: function(one, two){
						Cookie.write('localDevicesTabOpen', this.previous);
					}
				});
			break; //end devices
			case 'activeusers':
				$$(".kick").addEvent('click', function(){
					ajaxUpdateField('kick', 'ActiveUsers', { 'kick' : '1' }, this.get('id'));
				});
				$$(".ban").addEvent('click', function(){
					ajaxUpdateField('ban', 'ActiveUsers', { 'ban' : '1' }, this.get('id'));
				});
			break; //end activeusers
			case 'videoadj':
				$('backToList').addEvent('click', function(){
					var page = new DVRPage('devices');
				});
				var slider = new Array();
				$$('.slider').each(function(el){
					var id = el.get('id').toString();		
					slider[id] = new Slider(el, el.getElement('.knob'), {
						steps: 100,
						wheel: true,
						onChange: function(){
							switch (id){
								case 'hue': ajaxUpdateField('update_control', 'Devices', { 'hue' : this.step }, $('device_id').get('value'), 'none'); break;
								case 'brightness': ajaxUpdateField('update_control', 'Devices', { 'brightness' : this.step }, $('device_id').get('value'), 'none'); break;
								case 'saturation': ajaxUpdateField('update_control', 'Devices', { 'saturation' : this.step }, $('device_id').get('value'), 'none'); break;
								case 'audio_volume': ajaxUpdateField('update', 'Devices', { 'audio_volume' : this.step }, $('device_id').get('value'), 'none'); break;
								case 'contrast': ajaxUpdateField('update_control', 'Devices', { 'contrast' : this.step }, $('device_id').get('value'), 'none'); break;
							}
							$(id+'_value').set('value', this.step)
						}
					}).set($(id+'_value').get('value'));			
				});
				$('setToDefault').addEvent('click', function(){ 
					slider['hue'].set(50);
					slider['saturation'].set(50);
					slider['brightness'].set(50); 
					slider['audio_volume'].set(50); 
					slider['contrast'].set(50); 
				});
				$('liveViewImg').addEvent('load', function(){
					renewImg = function(){
						if ($('liveViewImg')){
							$('liveViewImg').set('src', ('/media/mjpeg.php?id='+$('device_id').get('value')+'&rnd='+new Date().getTime()));
						}
					}
					setTimeout("renewImg();", 500);
				});
				
			break;
			case 'deviceschedule':
				$('backToList').addEvent('click', function(){
					var page = new DVRPage('devices');
				});
				if (el = $('overrideGlobal')){
					el.addEvent('change', function(){					
						var newClass = (el.get('checked')) ? 'OK' : 'INFO';
						el.getParent().set('class', newClass);
					});
				};
				var grid = new localMotionGrid('scheduleContainer', 'valueString', 'deviceSchedule');
			break;//end deviceschedule
			case 'motionmap':
				$('backToList').addEvent('click', function(){
					var page = new DVRPage('devices');
				});
				var grid = new localMotionGrid('cameraOutput', 'valueString', 'mmap');
			break; //end motionmap
			case 'general':
				var generalForm  = new DVRSettingsForm('settingsForm'); 
			break;//end general
		}//end switch
	}
});

upadteResFps = function(el, ev){
	var devDiv = el.getParent('#localDevice');
	var id = el.get('id');
	var value = el.get('value');
	var mode = el.get('class');
	var request = new Request({
		url: 'ajax/update.php',
		data: 'mode='+mode+'&value='+value+'&id='+id,
		method: 'post',
		onRequest: function(){
			el.set('disabled', 'true');
			devDiv.setStyle('background-image', 'url("/img/loading.gif")');
		},
		onComplete: function(){
			el.set('disabled', '');
		},
		onSuccess: function(text, xml){
			var msg = xml.getElementsByTagName("msg")[0].childNodes[0].nodeValue;
			var status = xml.getElementsByTagName("status")[0].childNodes[0].nodeValue;
			var data = xml.getElementsByTagName("data")[0].childNodes[0].nodeValue;
			if (status=='OK'){
				devDiv.setStyle('background-image', 'url("/img/icons/check.png")');
				$$('.uc'+devDiv.getParent('#dvrCard').get('class')+' a').set('html', data);
			} else {
				devDiv.setStyle('background-image', 'url("/img/icons/cross.png")');
				var message = new DVRMessage(status, msg);
			}
			
		},
		onFailure: function(){
			var message = new DVRMessage('F', 'Could not communicate with the server at this time. Please make sure the server is running and you have an active connection.');
			devDiv.setStyle('background', 'url("/img/icons/cross.png") no-repeat right top');
		}
	}).send();
}

ajaxUpdateField = function(mode, type, values, id, page, data){
	var requestData = '';
	for (field in values){
		requestData += field+'='+values[field]+'&';
	}
	var request = new Request({
		url: 'ajax/update.php',
		data: 'mode='+mode+'&type='+type+'&'+requestData+'id='+id,
		method: 'post',
		onRequest: function(){
			if (page=='button'){
				$('saveButton').setStyle('background-image', 'url("/img/loading.gif")');
			}
		},
		onSuccess: function(text, xml){
			var msg = (xml.getElementsByTagName("msg")[0].childNodes[0].nodeValue || '');
			var status = (xml.getElementsByTagName("status")[0].childNodes[0].nodeValue || '');
			switch(status){
				case 'OK':
					if (page!='button' && page!='none' && page){
							var openPage = new DVRPage(page, data, status, msg);
					} else if(!page){
						var showMessage = new DVRMessage(status, msg);
					} else if(page=='button'){
						$('saveButton').setStyle('background-image', 'url("/img/icons/check.png")');
					}
				break;
				case 'F':
					if (page!='none'){
						var showMessage = new DVRMessage(status, msg);
						if (page=='button') { $('saveButton').setStyle('background-image', 'url("/img/icons/cross.png")'); };
					};
				break;
			}
		}
	}).send();
}

Element.implement({
	fadeAndDestroy: function(duration) {
		var el = this;
		this.set('tween', { duration: duration }).fade('out').get('tween').chain(function() { el.dispose(); });
	},
	deleteUserButton : function(){
		this.buttonAnimate('#edd');
		this.addEvent('click', function(){
			deleteUser($$('input[name=id]').get('value'), 'user');
		});
	},
	buttonAnimate : function(color) {
		var el = this;
		var original = (el.getStyle('background-color')=='transparent') ? '' : el.getStyle('background-color') ;
		var morph = new Fx.Morph(el, { 'duration':'100', link:'cancel' });
		el.addEvents({
			'mouseenter': function() { el.setStyle('cursor','pointer'); morph.start({ 'background-color': color }); },
			'mouseleave' : function() { el.setStyle('cursor',''); morph.start({ 'background-color': original });}
		});
	}
});

deleteUser = function(userID, type){
								var sureDelete = confirm('Delete this user (ID:'+userID+')?');
								if (sureDelete){
									var request = new Request({
										url: 'ajax/update.php',
										data: 'mode=deleteUser&id='+userID,
										method: 'post',
										onSuccess: function(text, xml){
											var msg = xml.getElementsByTagName("msg")[0].childNodes[0].nodeValue;
											var status = xml.getElementsByTagName("status")[0].childNodes[0].nodeValue;
											if (status=='OK' && type == 'list'){
												$$('#user-table #'+userID).fadeAndDestroy('1000');
											};
											if (status=='OK' && type == 'user') {
												var openPage = new DVRPage('users', '', status, msg);
											};
											var showMessage = new DVRMessage(status, msg);
										}
									}).send();
								}
}


DVRMessage = new Class({
	initialize: function(cs , msg){
		var el = new Element('div', {'id' : 'ajaxMessage', 'class' : cs, 'html' : msg+"<div class='bClear'></div>"});
		el.inject($('header'), 'before').highlight('#ff8c00');
		setTimeout("if ($('ajaxMessage')) { $('ajaxMessage').fadeAndDestroy('700'); };", 10000);
	}
});

function buttonMorph(el, color){
	var original = (el.getStyle('background-color')=='transparent') ? '' : el.getStyle('background-color') ;
	var morph = new Fx.Morph(el, { 'duration':'100', link:'cancel' });
	el.addEvents({
		'mouseenter': function() { el.setStyle('cursor','pointer'); morph.start({ 'background-color': color }); },
		'mouseleave' : function() { el.setStyle('cursor',''); morph.start({ 'background-color': original });}
	});
}

DVRSettingsForm = new Class({
	initialize: function(formID){
		$(formID).set('send', {
			onRequest: function(){
				$('saveButton').setStyle('background-image', 'url("/img/loading.gif")');
			},
			onComplete: function(text, xml){
				var msg = xml.getElementsByTagName("msg")[0].childNodes[0].nodeValue;
				var status = xml.getElementsByTagName("status")[0].childNodes[0].nodeValue;
				var iconStyle = (status=="OK") ? 'url("/img/icons/check.png")' : 'url("/img/icons/cross.png")';
				$('saveButton').setStyle('background-image', iconStyle);
				var showMessage = new DVRMessage(status, msg);
			}
		});
		$$('#'+formID).addEvent('click', function(){
			$('saveButton').fade('in');
			$('saveButton').setStyle('background-image', 'url("/img/icons/excl.png")');
		});
		$('saveButton').addEvent('click', function(){
			$(formID).send();	
		});
		buttonMorph($('saveButton'), '#8bb8');
	}
});

//image overlay/save class for motion detection page
var localMotionGrid = new Class({
		initialize: function(el, value, type){
			var self = this;
			switch(type){
				case 'mmap':
					var tempOverlay = new DVRContainerOverlay(); //takes time to load jpeg image
					$(el).addEvent('load', function(){
						self.drawGrid($(el), $(value).get('value'));
						tempOverlay.removeoverlay();
					});
					self.sensitivitySelector();
				break;//end mmap
				default:
					self.drawGrid($(el), $(value).get('value'), 24, 7);
					self.addScheduleFields();
					self.sensitivitySelector();
				break;
			};
			$('saveButton').addEvent('click', function(){
				self.saveSetting(self.prepareOutput($(el).getParent().get('id')), type);
			});

		},
		addScheduleFields :function(){
			hoursRow = new Element('tr', {'id' : row,'class' : 'gridRowEx'});
			for (i = -1; i<=23; i++){
				var text = (i==-1) ? '' : i;
				hourCol = new Element('td', {'id' : col, 'class' : 'gridColEx', 'html' : text, 'width': '25px'});
				hourCol.inject(hoursRow, 'bottom');
			}
			var weekDays = new Array('', 'Sunday', 'Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday', 'Saturday');
			hoursRow.inject($('gridTable'), 'top');
			$$('.gridRow').each(function(el){
				var text = weekDays[el.get('id')];
				dayCol = new Element('td', {'id' : col, 'class' : 'gridColEx', 'html' : text, 'width': '25px'});
				dayCol.inject(el, 'top');
			});
			
		},
		drawGrid: function(el, valueString, nc, nr, w, h){
			dragFlag = false;
			var numcols = (nc || el.getSize().x/16);
            var numrows = (nr || el.getSize().y/16);
			var gridTable = new Element('table', {
                'id' : 'gridTable',
                'styles' : {
                    'width' : (w || el.getSize().x),
                    'height' : (h || el.getSize().y)
                }
            });			
			for (row = 1; row<=numrows; row++){
                thisRow = new Element('tr', {'id' : row,'class' : 'gridRow'});
                for(col = 1; col<=numcols; col++){
					var colClassId = (valueString.substr((row-1)*numcols+col-1, 1) || 0);
                    thisCol = new Element('td', {'id' : col, 'class' : 'gridCol'+colClassId});
					thisCol.addEvents({
						'mousedown' : function(){
                            dragFlag = true;
							this.set('class', 'gridCol'+$$('#lvlSelect .on').get('id').toString().substring(2,3));
                            var startRow = this.getParent().get('id');
                            var startCol = this.get('id');
							
						},
						'mouseup' : function(){
							dragFlag = false;
						},
						'mouseenter' : function(){
							if (dragFlag){
								this.set('class', 'gridCol'+$$('#lvlSelect .on').get('id').toString().substring(2,3));
							 
							};
						}
					});
                    thisCol.inject(thisRow, 'bottom');
                };
                thisRow.inject(gridTable, 'bottom');
            };
            gridTable.inject(el.getParent());
		},
		
		//sets up seinsitivity selector
		sensitivitySelector: function(el){
			if ($('clearAll')){
				$('clearAll').addEvent('click', function(ev){
					ev.stopPropagation();
					$$('#cameraOutputContainer table tr td').set('class', 'gridCol0');
				});
				$('fillAll').addEvent('click', function(ev){
					$$('#cameraOutputContainer table tr td').set('class', 'gridCol'+$$('#lvlSelect .on').get('value'));
					ev.stopPropagation();
				});
			};
			$$('#lvlSelect ul li').each(function(el){
      	      if (el.get('id')!='clearAll' && el.get('id')!='fillAll') { el.addEvents({
       	         'click' : function(){
       	             $$('#lvlSelect ul li').set('class', '');
       	             this.set('class', 'on');
       	         }
            	});
				}
        	});
		},
		
		prepareOutput: function(el){	
			var stringToOutput = '';
			$$('#'+el+' table tr td').each(function(el){
				if (el.get('class')!='gridColEx') { stringToOutput += el.get('class').substr(7,1); };
			});
			return stringToOutput;
		},
		
		saveSetting: function(output, type){
			switch(type){
				case 'mmap': ajaxUpdateField('update', 'Devices', {'motion_map' : output}, $('cameraID').get('value'), 'button'); break;
				case 'deviceSchedule':
					if ($('cameraID').get('value')!='global'){
						if ($('overrideGlobal')) { ajaxUpdateField('update', 'Devices', {'schedule' : output, 'schedule_override_global' : $('overrideGlobal').get('checked')}, $('cameraID').get('value'), 'button'); }
						 else { ajaxUpdateField('update', 'Devices', {'schedule' : output}, $('cameraID').get('value'), 'button'); }
					} else {
						ajaxUpdateField('global', '', {'G_DEV_SCED' : output}, $('cameraID').get('value'), 'button');
					}
				break;
			};
		}
		
	});


//serverstat
updateStatData = function(){
		var statReq = new Request({
			url: 'ajax/stats.php',
			method: 'get',
			onSuccess: function(text, xml){
				var cpuUsage = xml.getElementsByTagName("cpu-usage")[0].childNodes[0].nodeValue;
				var memInUse = xml.getElementsByTagName("memory-inuse")[0].childNodes[0].nodeValue;
				var memTotal = xml.getElementsByTagName("memory-total")[0].childNodes[0].nodeValue;
				var memPertg = xml.getElementsByTagName("memory-used-percentage")[0].childNodes[0].nodeValue;
				var serverUp = xml.getElementsByTagName("server-uptime")[0].childNodes[0].nodeValue;
				$('serverStats').setStyle('display', 'block');
				updateStatBar('cpu', cpuUsage);
				updateStatBar('mem', memPertg);
				$('meminfo').set('html', Math.round(memInUse/1024) + "MB / " + Math.round(memTotal/1024)+ "MB");
				$('serverUptime').set('html', serverUp);

			}
		}).send();
		setTimeout("updateStatData();", 2000);
};

updateStatBar = function(barId, val, y, r){
	var yb = (y || 50);
	var rb = (r || 80);
	var el = $(barId);
	el.setStyle('background-position', (val-100));
	var pBarColor = 'green';
	if (val>=yb && val<rb){ pBarColor = 'yellow'; }
		else if(val>=rb) { pBarColor = 'red'; }
	el.setStyle('background-image', 'url("/img/pbar-'+pBarColor+'.png")');
	el.getChildren('.text').set('html', val+'%');
}




window.addEvent('domready', function(){
	containerDivID = 'pageContainer';
	
	var mainMenu = new DVRmainMenu();
	var pageToOpen = (Cookie.read('currentPage') || 'news');
	var pageToOpenData = (Cookie.read('currentPageData') || '');
	openPage = new DVRPage(pageToOpen, pageToOpenData);
	updateStatData();
});
