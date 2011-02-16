		liveViewerLayout = new Class({
			initialize: function(){
				var self = this;
				
				var lvRows = (Cookie.read('lvRows') || 2);
				var lvCols = (Cookie.read('lvCols') || 2);
				self.drawGrid(lvRows, lvCols);
			},
			
			drawGrid: function(r, c){
				
				var gridTable = new Element('table', {
   	            	'id' : 'lvGridTable'
   	        	});
				for (row = 1; row<=r; row++){
   	            	var thisRow = new Element('tr', {'id' : row,'class' : 'y'+row});
   	            	for(col = 1; col<=c; col++){
						//making sure that window does not overflow with content
						var imgSrcId = (Cookie.read('imgSrcy'+row+'x'+col) || 'none');
						imgSrcId = (imgSrcId!='none') ? '/media/mjpeg.php?multipart=true&id='+imgSrcId : '/img/icons/layouts/none.png';
							if ((480/(window.innerHeight-70-10*r)/r) > (704/(window.innerWidth-300-10*c)/c)){
								var imgHeight = (window.innerHeight-70-10*r>=r*480) ? '' : ((window.innerHeight-70-10*r)/r);
								if (imgSrcId=='none') {
									var imgWidth = 704/(480/((window.innerHeight-70-10*r)/r));
								}
							} else {
								var imgWidth = (window.innerWidth-230-10*c>=c*704) ? '' : ((window.innerWidth-300-10*c)/c);
								if (imgSrcId=='none') {
									var imgHeight = 480/(704/((window.innerWidth-300-10*c)/c));
								}
							};
						
						
   	                 	var thisCol = new Element('td', {'id' : col, 'class' : 'x'+col});
							
							var lvImg = new Element('img', {'class': 'lvImg' , 'width': imgWidth, 'height': imgHeight, 'src': imgSrcId});
							lvImg.inject(thisCol);
   	                	thisCol.inject(thisRow, 'bottom');
    	            };
       	        	thisRow.inject(gridTable, 'bottom');
       	    	};
        	    gridTable.inject($('liveViewContainer'));
			},
			
			addDelRowColumn: function(n, t){
				var tp = ((n)=='c' ? 'lvCols' : 'lvRows');
				var v = parseInt(Cookie.read(tp) || 2);
				Cookie.write(tp, ((t) ? v+1 : v-1));
				window.location.reload(true);
			},
			
			setLayout: function(tp){
				Cookie.write('lvCols', Math.sqrt(tp)); Cookie.write('lvRows', Math.sqrt(tp));
				window.location.reload(true);
			}
		});
		
		window.addEvent('domready', function(){
			var grid = new liveViewerLayout();
			$$('.ac').addEvent('click', function(){ grid.addDelRowColumn('c', true); });
			$$('.dc').addEvent('click', function(){ grid.addDelRowColumn('c', false); });
			$$('.ar').addEvent('click', function(){ grid.addDelRowColumn('r', true); });
			$$('.dr').addEvent('click', function(){ grid.addDelRowColumn('r', false); });
			
			$$('.l1').addEvent('click', function(){ grid.setLayout(1); });
			$$('.l4').addEvent('click', function(){ grid.setLayout(4); });
			$$('.l9').addEvent('click', function(){ grid.setLayout(9); });
			$$('.l16').addEvent('click', function(){ grid.setLayout(16); });
			//START
			$$('.device').each(function(el){
				el.addEvent('mousedown', function(ev){
					ev.stop();
					var dragOptions = {
						droppables: $$(''),
						onEnter: function(el, drop, ev){
							if(drop.get('class')=='lvImg') drop.set('class', 'lvImgOver');
						},
						onLeave: function(el, drop, ev){
							if(drop.get('class')=='lvImgOver') drop.set('class', 'lvImg');
						},
						onDrop: function(el, drop, ev){
							var id = el.get('id');
							el.dispose();
							if (drop && drop.get('class')=='lvImgOver'){
								drop.set('src', '/media/mjpeg.php?multipart=true&id='+id);
								drop.set('class', 'lvImg');
								//drop.set('height', ''); //previously assumed to be PAL for extra height
								var elParent = drop.getParent();
								Cookie.write('imgSrc'+elParent.getParent().get('class')+elParent.get('class'), id);
							}
							
							
						}
					};
					dragClone = this.clone(true, true).setStyles(this.getCoordinates()).setStyles({'opacity': 0.9, 'position': 'absolute'}).inject(document.body);
					var drag = dragClone.makeDraggable(dragOptions);
					drag.start(ev);

				});
			});	

		});
	
	
viewerSaveLoadLayout = new Class({
	initialize: function(){
		$('layouts').addEvent('change', function(){
			if (this.value==layoutToLoad) {
				var sts = true;
			} else {
				layoutsUpdate('load', this.value);
				var sts = false;
			}
			$('saveLayout').set('disabled', sts);
		});
		$('saveLayout').addEvent('click', function(){
			layoutsUpdate('edit', $('layouts').get('value'));
		});
		$('deleteLayout').addEvent('click', function(){
			layoutsUpdate('delete', $('layouts').get('value'));
		});
		$('clearAll').addEvent('click', function(){
			layoutsUpdate('clear', false);
		});
		$('saveLayoutAs').addEvent('click', function(){
			var layoutName = prompt('New layout name:');
			if (layoutName != null){
				layoutsUpdate('new', layoutName);
			}
		});
		
	}
});

layoutsUpdate = function(mode, layout){
	var request = new Request({
		url: 'liveview/layouts.php',
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

