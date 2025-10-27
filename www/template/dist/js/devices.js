$(function() {

    
    $('body').on("click", "#edittip-advanced-settings", function(e){
        var el = $('#edittip-advanced-settings-block');
        if (el.is(':visible')) el.hide();
        else el.show();
    });

    $('body').on("change", "#motionmap-algorithm :input", function() {
        if ($(this).val() == 0) {
            // default
            $('.bfh-slider[data-name="min_motion_area"]').addClass('disabled');
            $('.bfh-slider[data-name="max_motion_area"]').addClass('disabled');
            $('.bfh-slider[data-name="min_motion_frames"]').addClass('disabled');
            $('.bfh-slider[data-name="max_motion_frames"]').addClass('disabled');
            $('.bfh-slider[data-name="motion_blend_ratio"]').addClass('disabled');
        } else if ($(this).val() == 1) {
            // experimental
            $('.bfh-slider[data-name="min_motion_area"]').removeClass('disabled');
            $('.bfh-slider[data-name="max_motion_area"]').removeClass('disabled');
            $('.bfh-slider[data-name="min_motion_frames"]').addClass('disabled');
            $('.bfh-slider[data-name="max_motion_frames"]').addClass('disabled');
            $('.bfh-slider[data-name="motion_blend_ratio"]').addClass('disabled');
        } else if ($(this).val() == 2) {
            // OpenCV Temporal
            $('.bfh-slider[data-name="min_motion_area"]').removeClass('disabled');
            $('.bfh-slider[data-name="max_motion_area"]').removeClass('disabled');
            $('.bfh-slider[data-name="min_motion_frames"]').removeClass('disabled');
            $('.bfh-slider[data-name="max_motion_frames"]').removeClass('disabled');
            $('.bfh-slider[data-name="motion_blend_ratio"]').removeClass('disabled');
        }
    });

});

function deviceChangeNameSuccess(form, msg) {
    var cl = new deviceChangeName(form);
    cl.success();
}

var deviceChangeName = function(el) {
    var self = this;
    var els = {};

    self.success = function () {
        els.title.text(els.inp_device_name.val());
        self.form(0);
    }

    self.form = function(show) {
        if (show == 1) {
            els.parent.addClass('tmp-padding');

            els.title_bl.hide();
            els.form_bl.show();
        } else {
            els.parent.removeClass('tmp-padding');

            if (els.title.text() != els.inp_device_name.val()) els.inp_device_name.val(els.title.text());

            els.title_bl.show();
            els.form_bl.hide();
        }
    };

    var constructor = function () {
        els.parent = el.closest('.devices-device-name');

        els.title_bl = els.parent.find('.devices-device-name-title');
        els.title = els.title_bl.find('span');

        els.form_bl = els.parent.find('.devices-device-name-form');
        els.inp_device_name = els.form_bl.find('input[name="device_name"]');
    };
    constructor();
}

function scheduleOverrideGlobal(el) {
    var par = el.closest('div');
    var icon = par.find('i');
    var el_val = par.find('input[name="schedule_override_global"]');

    if (el.prop('checked')) {
        par.addClass('alert-success');
        par.removeClass('alert-warning');
        
        icon.addClass('fa-check-circle');
        icon.removeClass('fa-warning');

        el_val.val('1');
    } else {
        par.removeClass('alert-success');
        par.addClass('alert-warning');
        
        icon.removeClass('fa-check-circle');
        icon.addClass('fa-warning');

        el_val.val('0');
    }
}

function triggerONVIFevents(el) {
    var par = el.closest('div');
    var el_val = par.find('input[name="onvif_events_enabled"]');

    if (el.prop('checked')) {
        el_val.val('1');
        // Fetch and display live ONVIF event types
        var deviceId = $("input[name='id']").val();
        if (deviceId && deviceId !== 'global') {
            $('#onvif-live-event-types-list').html('<span class="text-info">Scanning for ONVIF events (20s)...</span>');
            $.get('/onvif/devices/' + deviceId + '/events/live-scan', function(response) {
                if (response && response.liveTopics && response.liveTopics.length > 0) {
                    var html = '<b>Detected ONVIF Event Types:</b><form id="onvif-event-types-form"><ul style="list-style:none;padding-left:0;">';
                    response.liveTopics.forEach(function(type, idx) {
                        var label = type.label || type.type;
                        var value = label;
                        html += '<li>' +
                            '<label>' +
                            '<input type="checkbox" class="onvif-event-checkbox" name="onvif_event_types[]" value="'+value+'" checked> ' +
                            '<b>' + label + '</b> <span style="color:#888">(' + type.fullTopic + ')</span>' +
                            '</label>' +
                            '</li>';
                    });
                    html += '</ul>';
                    html += '<input type="hidden" name="onvif_event_types_selected" id="onvif_event-types-selected" value="">';
                    html += '</form>';
                    $('#onvif-live-event-types-list').html(html);

                    // Update hidden input on change
                    function updateSelectedEvents() {
                        var selected = [];
                        $('#onvif-live-event-types-list .onvif-event-checkbox:checked').each(function() {
                            selected.push($(this).val());
                        });
                        $('#onvif_event-types-selected').val(selected.join(','));
                    }
                    $('#onvif-live-event-types-list').on('change', '.onvif-event-checkbox', updateSelectedEvents);
                    updateSelectedEvents();
                } else {
                    $('#onvif-live-event-types-list').html('<span class="text-warning">No ONVIF events detected during scan.</span>');
                }
            });
        }
    } else {
        el_val.val('0');
        $('#onvif-live-event-types-list').empty();
    }
}

function toggleMotionDebug(el) {
    var par = el.closest('div');
    var el_val = par.find('input[name="motion_debug"]');

    if (el.prop('checked')) {

        el_val.val('1');
    } else {

        el_val.val('0');
    }
}

function getMotionMap(form) {
    var tds = form.find('.table-grid td');
    var map = '';
    $.each(tds, function(i, val){
        var type = $(this).attr('data-type');
        map += type;
    });

    if (map) {
        $('#motion-map').val(map);
    }
        //$('#motion-map').val('map');
}

var motionGrid = function(el) {
    var self = this;
    var grid_bl = null;
    var table_grid = null;
    var table_grid_tds = null;
    var grid_color = null;
    var motion_map = null;
    var color_array = { 'bg-default' : 0 , 'bg-success' : 1, 'bg-info' : 2, 'bg-primary' : 3, 'bg-warning' : 4, 'bg-danger' : 5};
    var color_array_schedule = { 'bg-default' : 'N' , 'bg-success' : 'M', 'bg-info' : 2, 'bg-primary' : 'X', 'bg-warning' : 'T', 'bg-danger' : 'C'};
    var week_days = new Array('', 'Sunday', 'Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday', 'Saturday');

    var getColorClass = function (symb) {
        var res = 'bg-default';
        $.each(color_array, function(i, val){
            if (symb == val) res = i;
        });

        return res;
    };

    var gridChanges = function (el_tmp) {
        el_tmp = el_tmp || table_grid_tds;

        el_tmp.removeClass('bg-danger');
        el_tmp.removeClass('bg-warning');
        el_tmp.removeClass('bg-primary');
        el_tmp.removeClass('bg-info');
        el_tmp.removeClass('bg-success');
        el_tmp.removeClass('bg-default');

        el_tmp.addClass(grid_color);

        el_tmp.attr('data-type', color_array[grid_color]);
    };

    var btnChanges = function () {
        var btns = $('.motion-btn-sens');
        btns.addClass('disabled');
        el.removeClass('disabled');
    }

    var getGridColor = function () {
        var btn = $('.motion-sens-bl').find('button:not(.disabled)');
        grid_color = 'bg-'+btn.data('active');
    }

    var drawGrid = function (img, numcols, numrows) {
        var style = '';
        var el_class = '';
        if (img) {
            var width = img.width();
            var height = img.height();
            style = 'style="width: '+width+'px; height: '+height+'px;"';
            el_class = 'table-grid-opacity';
        } else {
            el_class = 'table table-condensed table-bordered table-schedule table-vert-align';
        }
    	var numcols = numcols || 32;
        var numrows = numrows || 24;

        var map_arr = motion_map.val();
        var map_arr = (""+map_arr).split("");

        var grid_table = '<table class="'+el_class+' table-grid" border="1" '+style+'>';

        if (!img) {
            grid_table += '<tr><thead>';
            grid_table += '<th></th>';
            for(col = 1; col<=numcols; col++) {
                grid_table += '<th>'+(col - 1)+'</th>';
            }
            grid_table += '</thead></tr>';
        }

        var cc = 0;
    	for (row = 1; row<=numrows; row++) {
            grid_table += '<tr>';
            if (!img) {
                grid_table += '<th>'+week_days[row]+'</th>';
            }

            for(col = 1; col<=numcols; col++) {
                var td_tmp = '<td class="'+getColorClass(map_arr[cc])+'" data-type="'+map_arr[cc]+'"></td>';
                grid_table += td_tmp;

                cc++;
            }

            grid_table += '</tr>';
        };

        grid_table += '</table>';

        grid_bl.append(grid_table);


        $('body').on("mousedown", ".table-grid td", function(e){
            var mg = new motionGrid(null);
            if (!img) mg.setSchedule();
            mg.getGridColorP();

            $('body').on("mousemove", ".table-grid td", function(e){
                mg.gridChangesP($(this));

            }).on('mouseup', function(e) {
                $('body').off("mousemove", ".table-grid td");
            });
        });

        $('body').on("click", ".table-grid td", function(e){
            var mg = new motionGrid(null);
            if (!img) mg.setSchedule();
            mg.getGridColorP();
            mg.gridChangesP($(this));
        });
    }

    self.sheduleDrawGrid = function () {
        drawGrid(null, 24, 7);
    }

    self.setSchedule = function () {
        color_array = color_array_schedule;
    }

    self.getGridColorP = function () {
        getGridColor();
    };

    self.gridChangesP = function (el_tmp) {
        gridChanges(el_tmp);
    }

    self.off = function () {
        grid_color = 'bg-default';
        el.data('active', 'default');
        btnChanges();
    };

    self.minimal = function () {
        grid_color = 'bg-success';
        el.data('active', 'success');
        btnChanges();
    };

    self.low = function () {
        grid_color = 'bg-info';
        el.data('active', 'info');
        btnChanges();
    };

    self.average = function () {
        grid_color = 'bg-primary';
        el.data('active', 'primary');
        btnChanges();
    };

    self.high = function () {
        grid_color = 'bg-warning';
        el.data('active', 'warning');
        btnChanges();
    };

    self.veryHigh = function () {
        grid_color = 'bg-danger';
        el.data('active', 'danger');
        btnChanges();
    };

    self.showHideImage = function () {
        var img = grid_bl.find('img');
        if (img.is(':visible')) img.hide();
        else img.show();
    };
    
    self.clearAll = function () {
        grid_color = 'bg-default';
        gridChanges();
    };
    
    self.fillAll = function () {
        getGridColor();
        gridChanges();
    };

    self.initDrawGrid = function () {
        var img = grid_bl.find('img');
        var refresh_bl = grid_bl.find('.glyphicon-refresh');

        var driver = img.data('driver');

        img.on('load', function() { 
                img.addClass('img-active');
                refresh_bl.hide();

                if (driver == 'tw5864') drawGrid(img, 16, 12);
		else if (driver == 'solo6110' || driver == 'solo6010') {
			var signaltype = img.data('signaltype');

			if (signaltype == 'pal')
				drawGrid(img, 22, 18);
			else if (signaltype == 'ntsc')
				drawGrid(img, 22, 15);
			else
				drawGrid(img, 22, 15);
		}
                else drawGrid(img, 32, 24);//32x24 - motion map size for IP cams
            }).on('error', function() { 
                img.addClass('img-active');
                refresh_bl.hide();

                if (driver == 'tw5864') drawGrid(img, 16, 12);
                else if (driver == 'solo6110' || driver == 'solo6010') {
                        var signaltype = img.data('signaltype');

                        if (signaltype == 'pal')
                                drawGrid(img, 22, 18);
                        else if (signaltype == 'ntsc')
                                drawGrid(img, 22, 15);
                        else
                                drawGrid(img, 22, 15);
                }
                else drawGrid(img, 32, 24);//32x24 - motion map size for IP cams

            }).attr("src", img.attr("src"));
    };

    var constructor = function () {
        grid_bl = $('.grid-bl');
        table_grid = $('.table-grid');
        table_grid_tds = $('.table-grid td');
        motion_map = $('#motion-map');
    };
    constructor();
}

function delCamera(form) {
    form.closest('.well').remove();
}

function deviceSwith(form, msg) {
    // okay
    if (msg.status == 6) {
        var par = form.closest('.well');
        var par_col = par.parent();
        var icon = par.find('.devices-device-name i');
        var submit = par.find('button.devices-onoff-form');
        var disabled_bl = $('#devices-ip-disabled');
        var ok_bl = $('#devices-ip-ok');

        if (icon.hasClass('text-status-OK')) {
            icon.removeClass('text-status-OK');
            icon.addClass('text-status-disabled');
            submit.data('complete-text', $('#devices-text-status-disabled').text());

            par_col.detach().appendTo(disabled_bl);

            disabled_bl.show();
        } else {
            icon.removeClass('text-status-disabled');
            icon.addClass('text-status-OK');
            submit.data('complete-text', $('#devices-text-status-OK').text());

            par_col.detach().appendTo(ok_bl);

            if (disabled_bl.find('div').length < 1) disabled_bl.hide();
        }

    } else {
        // error

    }
}

function devicesReloadPage(form, msg) {
    var par = form.closest('.panel-collapse');
    var par = par.attr('id');

    var ajax_req = new ajaxReq();
    ajax_req.ajaxContent('/ajax/devices.php', {
            callback_func: function (msg, done) {
                if (done) {
                    par = $('#'+par);
                    par.collapse({toggle: false });
                    par.collapse('show');
                }
            }
    });
}


var devicesCards = function(el) {
    var self = this;
    var els = {};
    var msg = null;

    self.setCapacity = function () {
        els.par_panel.find('.devices-cards-capacity').html(msg.data);
    };

    self.setData = function (data) {
        msg = data;
    };

    var constructor = function (el) {
        els.par_well = el.closest('.well');
        els.par_panel = el.closest('.panel-collapse');
    };
    constructor(el);
}

