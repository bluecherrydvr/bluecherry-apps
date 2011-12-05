DVRmainMenu = new Class({
	initialize: function(){
		$$('#menuButtons li').each(function(el){
			var morph = new Fx.Morph(el,{ 'duration':'200', link:'cancel' });
			var original = (el.getStyle('background-color')=='transparent') ? '#fff' : el.getStyle('background-color') ;
			el.addEvents({
				'mouseenter' : function() { morph.start({ 'background-color':'#ff8c00' }); el.setStyle('cursor', 'pointer');},
				'mouseleave' : function() { morph.start({ 'background-color': original }); el.setStyle('cursor', ''); },
				'click'		 : function(ev) { 
												if (el.get('class')=='liveView') { window.location.href = '/?l=true'; return false; };
												if (el.get('class')=='downloadClient') { window.location.href = var_rm_client_download; return false; };  
												ev.preventDefault(); 
												openPage = new DVRPage(this.get('id')); }

			});
		});
		if ($('lmNewVersion')){
			$('lmNewVersion').addEvent('click', function(){
				var openPage = new DVRPage('newversion');
			});
		}
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
		if ($('pageContainerLoadingOverlay')){
			$('pageContainerLoadingOverlay').dispose();
		}
	}
});



DVRPageScript = new Class({
	initialize: function(page){
		switch (page){
			case 'ptzpresetedit':
				$('backToList').addEvent('click', function(){
					var page = new DVRPage('devices');
				});
				$('backToPresetList').addEvent('click', function(){
					var page = new DVRPage('ptzpresetlist', 'id='+$('ref').get('value'));
				});
				var userForm = new DVRSettingsForm('settingsForm');
			break;
			case 'ptzpresetlist':
				$$('.add').addEvent('click', function(ev){
					var page = new DVRPage('ptzpresetedit', 'id=new&ref='+$('cameraId').get('value'));
				});
				$$('.preset').addEvent('click', function(ev){
					if (selected = $('selected')){ 
						selected.set('id', '');
					}
					this.set('id', 'selected');
					var protocol = (this.getChildren('.name').get('id')) ? 'PELCO' : '';
					ajaxUpdateField('update', 'Devices', {'ptz_control_path' : this.getChildren('.name').get('id'), 'ptz_control_protocol' : 'PELCO' }, $('cameraId').get('value'), false);
				});
				$$('.delete').addEvent('click', function(ev){
					ev.stopPropagation();
					var sureDelete = confirm(var_del_ipp_conf+this.getParent().getChildren('.name').get('html')+'"?');
					if (sureDelete){
						ajaxUpdateField('deleteIpPtzPreset', 'Devices', {'do' : true }, this.getParent().getChildren('.name').get('id'), 'ptzpresetlist', 'id='+$('cameraId').get('value'));
					};
				});
				$$('.edit').addEvent('click', function(ev){
					ev.stopPropagation();
					var page = new DVRPage('ptzpresetedit', 'id='+this.getParent().getChildren('.name').get('id')+'&ref='+$('cameraId').get('value'));
				});
				$('backToList').addEvent('click', function(){
					var page = new DVRPage('devices');
				});
			break; //end ptzpresets
			case 'statistics':
				$('sForm').set('send', { 
					onComplete: function(text, xml){
						$('sResults').set('html', text);
					},
					onFailure: function(){
						$('sResults').set('html', 'Could not complete request, please try later.');
					}
				});
				$('formSubmit').addEvent('click', function(e){
					e.preventDefault();
					$('sForm').send();
				});
			break;
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
					if ($('deleteButton') != null) {
						$('deleteButton').deleteUserButton();
					}
				};
				if ($('editAccessList')!=null){
					$('editAccessList').addEvent('click', function(){
						openPage = new DVRPage('cameraperms', 'id='+$$('.objId').get('value'));
					});
					$('editAccessList').buttonAnimate("#aac");
				}
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
				} else {
					buttonMorph($('addEmail'), '#8bb8');
					$('addEmail').addEvent('click', function(){
						var el = new Element('span', {'html': "<span><label>"+var_email+"<span class='sub'>"+var_email_ex+"</span></label><input type='text' class='email' name='email[]' value='' /><input type='text' class='limit' name='limit[]' value='0' /></span><div class='bClear'></div><br />"});
						el.inject($('addEmail'), 'before');
					});
				}
			break;//end users
			case 'ptzsettings':
				var ptzSettingsForm = new DVRSettingsForm('settingsForm');
				$('backToList').addEvent('click', function(){
					var page = new DVRPage('devices');
				});
			break;
			case 'addip':
				if ($('updatelist')){
					$('updatelist').addEvent('click', function(){
						var request = new Request({
							url: 'ajax/updateiplist.php',
							method: 'post',
							onRequest: function(){
							},
							onSuccess: function(html, xml){
								var msg = xml.getElementsByTagName("msg")[0].childNodes[0].nodeValue;
								var status = xml.getElementsByTagName("status")[0].childNodes[0].nodeValue;
								var showMessage = new DVRMessage(status, msg);
								if (status=='OK'){
									$('updatelistContainer').dispose();
								}
							}
						}).send();
					});
				};
				expandAdvancedSettings = function(a){
					var s = (($('advancedSettings').getStyle('display') == 'none' && a!='close') || (a=='open')) ? 'block' : 'none';
					$('advancedSettings').setStyle('display', s);
				}
				$('advancedSettingsSwitch').addEvent('click', function(){
					expandAdvancedSettings();
				});
				getInfo = function(t, m, x, containerId){
					var request = new Request({
						url: 'ajax/addip.php',
						data: 'm='+t+'&'+m+'='+x,
						method: 'get',
						onRequest: function(){
						},
						onSuccess: function(html, xml){
							if (containerId) { 
								$(containerId).set('html', html)}
							else {
								var mjpegPath = (xml.getElementsByTagName("mjpegPath")[0].childNodes[0].nodeValue || '');
								var rtspPath = (xml.getElementsByTagName("rtspPath")[0].childNodes[0].nodeValue || '');
								var mjpegPort = (xml.getElementsByTagName("mjpegPort")[0].childNodes[0].nodeValue || '');
								var rtspPort = (xml.getElementsByTagName("rtspPort")[0].childNodes[0].nodeValue || '');
								$('mjpeg').set('value', mjpegPath);
								$('rtsppath').set('value', rtspPath);
								$('port').set('value', rtspPort);
								$('mjpegPort').set('value', mjpegPort);
								if (mjpegPath=='' || rtspPath == '' || mjpegPort== ''){ //in case paths are not in driver DB or empty
									expandAdvancedSettings('open');
								} else {
									expandAdvancedSettings('close');
								}
							}
							switch (containerId){
								case 'modelSelector':
									$('models').addEvent('change', function(){
										if (this.value!='Please choose model'){
											var w = false;
											if (this.value == 'Generic'){
												expandAdvancedSettings('open');
											} else {
												getInfo('ops', 'model', this.value, false);
											};
										} else {
											var w = true;
										}
										$$('#aip input').set('disabled', w);
									});
								break;
							}
						}
					}).send();
				}
				$('manufacturers').addEvent('change', function(){
					getInfo('model', 'manufacturer' , this.value, 'modelSelector');
					if (this.value == 'Please choose manufacturer') $$('#aip input').set('disabled', true); //if reset
				});
				buttonMorph($('saveButton'), '#8bb8');
				$('settingsForm').set('send', {
					onRequest: function(){
						$('saveButton').setStyle('background-image', 'url("/img/loading.gif")');
					},
					onComplete: function(text, xml){
						
						var msg = xml.getElementsByTagName("msg")[0].childNodes[0].nodeValue;
						var status = xml.getElementsByTagName("status")[0].childNodes[0].nodeValue;
						var iconStyle = (status=="OK") ? 'url("/img/icons/check.png")' : 'url("/img/icons/cross.png")';
						if (status=="OK"){
							var page = new DVRPage('devices', '', status, msg);
						} else {
							var showMessage = new DVRMessage(status, msg);
						}
						$('saveButton').setStyle('background-image', iconStyle);
					}
				});
				$('saveButton').addEvent('click', function(ev){
					ev.preventDefault();
					$('settingsForm').send();
				});
			break;
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
					openPage = new DVRPage('log', 'lines='+this.get('value')+'&type='+$('type').get('value'));
				});
				$('type').addEvent('change', function(){
					openPage = new DVRPage('log', 'type='+this.get('value'));
				});
			break; //end log
			case 'storage':
				var storageForm  = new DVRSettingsForm('storageForm');
				var addButton = $$('#button.add');
				addButton.buttonAnimate("#aca");
				var initDelete = function(){
					$$('.deleteShort').each(function(el){
						el.buttonAnimate("#caa");
						el.addEvent('click', function(ev){
							el.getParent().dispose();
						});
					});
				};
				initDelete();
				addButton.addEvent('click', function(ev){
					var el = new Element('div', {'id': 'separator', 'html': $('tableEntry').get('html')});
					el.inject($('storageForm'));
					initDelete();
				});
				
			break; //end storage
			case 'devices' :
				if (ipAttn = $$('.ipAttn')){
					ipAttn.each(function(el){
						el.addEvents({
							'mouseover': function(){
								this.getParent().getChildren('.ipCamInfo').setStyle('display', 'inline');
								this.getParent().getParent().setStyle('overflow', '');
							},
							'mouseout': function(){
								this.getParent().getChildren('.ipCamInfo').setStyle('display', 'none');
								this.getParent().getParent().setStyle('overflow', 'hidden');
							}
						});
					});
				}
				if ($$('.enableAll')){
					$$('.enableAll').addEvent('click', function(){
						ajaxUpdateField('enableAll', 'Devices', {'card_id' : this.get('id')}, this.get('id'), 'devices');
					});
				};
				$('addIPCamera').addEvent('click', function(){
					openPage = new DVRPage('addip');
				});
				$$('.ipDeviceSchedule').each(function(el){
					el.addEvent('click', function(){
						var page = new DVRPage('deviceschedule', 'id='+el.get('id'));
					});
				});
				$('addIPCamera').buttonAnimate("#aca");
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
				var context = new ContextMenu({
					offsets: { x:-250, y:-50 },
					targets: '.settingsOpen',
					trigger: 'click',
					menu:	'settingsMenu',
					actions: {
						'videoadj':function(el, ref){
							var page = new DVRPage('videoadj', 'id='+el.get('id'));
						},
						'editmap':function(el, ref){
							var page = new DVRPage('motionmap', 'id='+el.get('id'));
						},
						'deviceschedule':function(el, ref){
							var page = new DVRPage('deviceschedule', 'id='+el.get('id'));
						},
						'ptzsettings':function(el, ref){
							var page = new DVRPage('ptzsettings', 'id='+el.get('name'));
						}
					}
				});
				var ipContext = new ContextMenu({
					offsets: { x:-250, y:-50 },
					targets: '.ipSettingsOpen',
					trigger: 'click',
					menu:	'ipSettingsMenu',
					actions: {
						'delete':function(el, ref){
							var sureDelete = confirm(var_del_can_conf+el.get('id')+')?');
							if (sureDelete){
								ajaxUpdateField('deleteIp', 'Devices', {'do' : true }, el.get('id'), 'devices');
							};
						},
						'properties':function(el, ref){
							var page = new DVRPage('editip', 'id='+el.get('id'));
						},
						'deviceschedule':function(el, ref){
							var page = new DVRPage('deviceschedule', 'id='+el.get('id'));
						},
						'ptzsettings':function(el, ref){
							var page = new DVRPage('ptzpresetlist', 'id='+el.get('id'));
						},
						'oncam':function(el, ref){
							window.open('/ajax/redirect.php?id='+el.get('id'));
						},
						'editmap':function(el, ref){
							var page = new DVRPage('motionmap', 'id='+el.get('id'));
						}
					}
				})
				$$('.change_state').each(function(el){
					el.addEvent('click', function(){
						var mode = (el.getParent().getParent().get('id') == 'ipDevice') ? 'changeStateIp' : 'changeState';
						ajaxUpdateField(mode, 'Devices', {'do' : true }, el.get('id'), 'devices');
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
			case 'editip':
				buttonMorph($('saveButton'), '#8bb8');
				$('settingsForm').set('send', {
					onRequest: function(){
						$('saveButton').setStyle('background-image', 'url("/img/loading.gif")');
					},
					onComplete: function(text, xml){
						
						var msg = xml.getElementsByTagName("msg")[0].childNodes[0].nodeValue;
						var status = xml.getElementsByTagName("status")[0].childNodes[0].nodeValue;
						var iconStyle = (status=="OK") ? 'url("/img/icons/check.png")' : 'url("/img/icons/cross.png")';
						var showMessage = new DVRMessage(status, msg);
						$('saveButton').setStyle('background-image', iconStyle);
					}
				});
				expandAdvancedSettings = function(a){
					var s = (($('advancedSettings').getStyle('display') == 'none' && a!='close') || (a=='open')) ? 'block' : 'none';
					$('advancedSettings').setStyle('display', s);
				}
				$('advancedSettingsSwitch').addEvent('click', function(){
					expandAdvancedSettings();
				});
				$('saveButton').addEvent('click', function(ev){
					ev.preventDefault();
					$('settingsForm').send();
				});
				$('backToList').addEvent('click', function(){
					var page = new DVRPage('devices');
				});
			break;
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
				$("audio_enabled").addEvent('change', function(){
					ajaxUpdateField('update_control', 'Devices', { 'audio_disabled' : ($("audio_enabled").get('checked')) ? 0:1 }, $('device_id').get('value'), 'none');
				});
			break;
			case 'cameraperms':
				$('userInfo').addEvent('click', function(){
					openPage = new DVRPage('users', 'id='+this.get('class'));
				});
				$('allUsers').addEvent('click', function(){
					openPage = new DVRPage('users');
				});
				$$('#camList div').addEvent('click', function(){
					var newClass = (this.get('class')=='allowed') ? 'nallowed' : 'allowed';
					this.set('class', newClass);
				});
				var sb = $('saveButton');
				sb.buttonAnimate("#aca");
				sb.addEvent('click', function(){
					var naList = '';
					$$('#camList .nallowed').each(function(el){
						naList += el.get('id')+',';
					});
					ajaxUpdateField('access_device_list', 'Users', { 'value' : naList }, $('userInfo').get('class'), 'button');
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
						//ajaxUpdateField('update', 'Devices', {'buffer_prerecording' : this.value}, $('cameraID').get('value'), 'button');
					});
				};
				var grid = new localMotionGrid('scheduleContainer', 'valueString', 'deviceSchedule');
			break; //end deviceschedule
			case 'motionmap':
				$('buffer_prerecording').addEvent('change', function(){
					ajaxUpdateField('update', 'Devices', {'buffer_prerecording' : this.value}, $('cameraID').get('value'), 'button');
				});
				$('buffer_postrecording').addEvent('change', function(){
					ajaxUpdateField('update', 'Devices', {'buffer_postrecording' : this.value}, $('cameraID').get('value'), 'button');
				});
				var imgOn = true;
				$('backToList').addEvent('click', function(){
					var page = new DVRPage('devices');
				});
				$$('.showHideImg')[0].addEvent('click', function(ev){
					if (imgOn) {
						$('cameraOutput').fade('out');
						imgOn = false;
					} else {
						$('cameraOutput').fade('in');
						imgOn = true;
					}
					ev.stopPropagation();
				});
				if ($('cameraType').value == 'IP'){
					var grid = new localMotionGrid('cameraOutput', 'valueString', 'mmap', 32, 24);
				} else {
					var grid = new localMotionGrid('cameraOutput', 'valueString', 'mmap');
				}
				
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

settingsForm = new Class({
	initialize: function(formId, saveButton){
		$(formId).set('send', { 
			onRequest: function(){
				$$(saveButton).setStyle('background-image', 'url("/img/loading.gif")');
			},
			onComplete: function(text, xml){
				var msg = xml.getElementsByTagName("msg")[0].childNodes[0].nodeValue;
				var status = xml.getElementsByTagName("status")[0].childNodes[0].nodeValue;
				var iconStyle = (status=="OK") ? 'url("/img/icons/check.png")' : 'url("/img/icons/cross.png")';
				$$(saveButton).setStyle('background-image', iconStyle);
				$$(saveButton).highlight();
				var showMessage = new DVRMessage(status, msg);
			}
		});
		$$(saveButton).addEvent('click', function(){
			$(formId).send();	
		});
		buttonMorph($$(saveButton), '#8bb8');
	}
});

DVRSettingsForm = new Class({
	initialize: function(formID){
		$(formID).set('send', { 
			onRequest: function(){
				var savebutton = ($('saveButton')) ? $('saveButton') : $('.save'); 
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
		initialize: function(el, value, type, nc, nr){
			var self = this;
			switch(type){
				case 'notifications':
				break; //end notifications
				case 'mmap':
					var tempOverlay = new DVRContainerOverlay(); //takes time to load jpeg image
					$(el).addEvent('load', function(){
						self.drawGrid($(el), $(value).get('value'), nc, nr);
						tempOverlay.removeoverlay();
						self.sensitivitySelector();
					});
				break;//end mmap
				default:
					this.drawGrid($(el), $(value).get('value'), 24, 7);
					this.addScheduleFields();
					this.sensitivitySelector();
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
            gridTable.addEvent('mouseleave', function(){
				dragFlag = false;
			});
            gridTable.inject(el.getParent());

		},
		
		//sets up seinsitivity selector
		sensitivitySelector: function(el){
			if ($('clearAll')){
				
				$('clearAll').addEvent('click', function(ev){
					ev.stopPropagation();
					$$('#cameraOutputContainer table tr td').set('class', 'gridCol0');
				});
			}
			if ($('fillAll')){
				$('fillAll').addEvent('click', function(ev){
					$$('#gridTable tr td:not(td.gridColEx)').set('class', 'gridCol'+$$('#lvlSelect .on').get('id').toString().substring(2,3));
					$$('#cameraOutputContainer table tr td').set('class', 'gridCol'+$$('#lvlSelect .on').get('id').toString().substring(2,3));
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
						if ($('overrideGlobal')) { var v = $('overrideGlobal').get('checked') ? 1 : 0; ajaxUpdateField('update', 'Devices', {'schedule' : output, 'schedule_override_global' : v}, $('cameraID').get('value'), 'button'); }
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
				var cpuUsage = (xml.getElementsByTagName("cpu-usage")[0].childNodes[0].nodeValue || 0);
				var memInUse = (xml.getElementsByTagName("memory-inuse")[0].childNodes[0].nodeValue || 0);
				var memTotal = (xml.getElementsByTagName("memory-total")[0].childNodes[0].nodeValue || 0);
				var memPertg = (xml.getElementsByTagName("memory-used-percentage")[0].childNodes[0].nodeValue || 0);
				var serverUp = (xml.getElementsByTagName("server-uptime")[0].childNodes[0].nodeValue || 0);
				var serverRn = (xml.getElementsByTagName("bc-server-running")[0].childNodes[0].nodeValue || 0);
				var writeFail= (xml.getElementsByTagName("failed-write")[0].childNodes[0].nodeValue || 0);
				var sr  = $('sr');
				var snr = $('snr');
				var ncn = $('ncn');
				var ftw = $('ftw');
				if (serverRn != 'up'){
					sr.setStyle('display', 'none');
					snr.setStyle('display', 'inline');
					snr.getChildren().highlight('#faa');
				} else {
					snr.setStyle('display', 'none');
					sr.setStyle('display', 'inline');
				}
				if (writeFail!='false'){
					$('writeFailTime').set('html', writeFail); 
					ftw.setStyle('display', 'inline');
				} else {
					ftw.setStyle('display', 'none');
				}
				ncn.setStyle('display', 'none');
				$('serverStats').setStyle('display', 'block');
				updateStatBar('cpu', cpuUsage);
				updateStatBar('mem', memPertg);
				$('meminfo').set('html', Math.round(memInUse/1024) + "MB / " + Math.round(memTotal/1024)+ "MB");
				$('serverUptime').set('html', serverUp);

			},
			onFailure: function(){
				var sr  = $('sr');
				var snr = $('snr');
				var ncn = $('ncn');
				ncn.setStyle('display', 'inline');
				sr.setStyle('display', 'none');
				snr.setStyle('display', 'none');
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





