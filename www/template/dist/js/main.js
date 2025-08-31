$(function() {


    ajaxEvent();

    window.history.pushState({path: window.location.href}, '');
});

// back button browser
$(window).on("popstate", function () {
    if (history.state) {
        var url = history.state.path;
        var ajax_req = new ajaxReq();
        ajax_req.ajaxContent(url, {
            ajax_content_pushstate: false
        });

        // reactive left menu item
        var menus = $('#side-menu li a');
        menus.removeClass('active');
        menus.each(function(i) {
            var menu_url = $(this).attr('href');
            if (url.indexOf(menu_url) != -1) {
                $(this).addClass('active');
                $(this).focus();
                return false;
            }
        });
    }
});

function tooltip() {
    $("[data-toggle=popover]").popover({
        html: true,
        trigger: 'hover',
        placement: function (tip, element) {
            var my_left = $(element).offset().left;
            if (my_left < 500) return 'left';
            return 'right';
        }
    });
}

function whereToPlace() {
    var my_left = $(this).offset.right;
    //alert(my_left);
    if (my_left > 500) return 'right';
    return 'left';
}


function commons() {

    $('body').on("click", ".downloadClient", function(e){
        e.preventDefault();
        window.open(var_rm_client_download);
    });
    
    $('body').on("click", ".liveView", function(e){
        e.preventDefault();
        window.location.href = '/liveview';
    });
}

function protocolBlocks (el) {
    var rtsp = $('.devices-rtsp-setting');
    var mjpeg = $('.devices-mjpeg-setting');

    if (el.find('option:selected').val() == 'IP-RTSP') {
        rtsp.show();
        mjpeg.hide();
    } else {
        rtsp.hide();
        mjpeg.show();
    }
}

// MODERN STATS UPDATE SYSTEM - Simple and robust
function updateStatData() {
    // Simple fetch request with error handling
    fetch('/ajax/stats.php?format=json')
        .then(response => {
            if (!response.ok) {
                throw new Error('Network response was not ok');
            }
            return response.json();
        })
        .then(data => {
            // Hide connection error
            $('#ncn').hide();
            
            // Update server status
            if (data.live && data.live.server_running) {
                $('#sr').show();
                $('#snr').hide();
                $('#server-stats').show();
                
                // Update stats
                if (data.live.cpu !== undefined) {
                    updateStatBar('stat-cpu', data.live.cpu);
                }
                if (data.live.memory !== undefined) {
                    updateStatBar('stat-mem', data.live.memory);
                }
                if (data.live.memory_info) {
                    $('#stat-meminfo').html(data.live.memory_info);
                }
                if (data.live.uptime) {
                    $('#server-uptime').html(data.live.uptime);
                }
            } else {
                // Server not running
                $('#sr').hide();
                $('#snr').show();
                $('#server-stats').hide();
            }
        })
        .catch(error => {
            console.log('Stats update failed:', error);
            // Show connection error
            $('#ncn').show();
            $('#sr').hide();
            $('#snr').hide();
            $('#server-stats').hide();
        });
    
    // Schedule next update
    setTimeout(updateStatData, 5000); // Update every 5 seconds
}

updateStatBar = function(barId, val, y, r){
	var yb = (y || 50);
	var rb = (r || 80);
	var el = $('#'+barId);
    var progress_bar = el.find('.progress-bar');

    //alert(val_calc);
    
	progress_bar.css('width', val+'%');
    progress_bar.attr('aria-valuenow', val);

    var pBarColor = 'progress-bar-success';
    if (val >= yb && val < rb){ pBarColor = 'progress-bar-warning'; }
    else if (val >= rb) { pBarColor = 'progress-bar-danger'; }

    progress_bar.removeClass('progress-bar-success');
    progress_bar.removeClass('progress-bar-warning');
    progress_bar.removeClass('progress-bar-danger');

    progress_bar.addClass(pBarColor);
    //el.css('background-image', 'url("/img/pbar-'+pBarColor+'.png")');

    el.find('span').html(val+'%');
}

function delTr(form) {
    form.closest('tr').remove();
}

var ajaxReq = function () {
    var self = this;
    var el_clicked = null;
    var send_but = null;
    var form = null;
    var form_act = null;
    var form_data = null;
    var alert_bl = null;
    var data_send = null;
    var error_ajax = null;
    var process_data = false;
    var content_type = false;
    var redir = false;
    var ajax_content = false;
    var ajax_content_pushstate = true;
    var type_req = "POST";
    var type_data = "JSON";
    var callback_func = null;
    var func_after = null;
    var class_after = null;
    var redirect_after = null;
    var redirect_after_success = null;
    var func_after_success = null;
    var class_after_success = null;


    var send = function (redirectOnFail) {
        if (form !== null) form_data =  new FormData(form[0]);

        if (alert_bl !== null) {
            alert_bl.hide();
        }
        
        //$.ajax({
            //type: "POST",
            //url: form_act,
            ////data: form.serialize(),
            //data: form_data,
            //processData: false,
            //contentType: false,
            //dataType: "JSON",
            //xhrFields: {
                //// add listener to XMLHTTPRequest object directly for progress (jquery doesn't have this yet)
                //onprogress: function(progress) {
                //}
            //},            
            //success: function(msg){
                //respProc(msg);
            //},
            //error: function (msg) {
                //if (error_ajax) alert('err ajax');
                ////alert('err ajax');
            //},
            //timeout: function (msg) {
                ////if (error_ajax) alert('err ajax');
                //alert('timeout');
            //},
            //beforeSend: function() {
                //send_but.trigger("start.search");
            //},
            //complete: function() {
                //if (!redir) send_but.trigger("finish.search");
            //}            
      //});
        var hz = $.ajax({
                type: type_req,
                url: form_act,
                //data: { id : 'menuId' },
                //data: form.serialize(),
                data: form_data,
                processData: process_data,
                contentType: content_type,
                dataType: type_data,
                xhrFields: {
                    // add listener to XMLHTTPRequest object directly for progress (jquery doesn't have this yet)
                    onprogress: function(progress) {
                    }
                },            
                beforeSend: function() {
                    if (send_but !== null) {
                        if (ajax_content) send_but.trigger("start_content.search");
                        else send_but.trigger("start.search");
                    }
                }
            })
            .done(function(msg) {
                respProc(msg);
            })
            .fail(function(msg) {
                if(redirectOnFail) {
                    // This is workaround for ublockORIGIN browser plugin which blocks get HTML requests(logs page - issue #474).
                    console.error('Request failed redirecting to ' + form_act)
                    location.href = location.origin + form_act
                }

                if (error_ajax) alert('err ajax');

                if ((callback_func !== null) && (typeof(callback_func === 'function'))) {
                    callback_func(msg, false);
                }
            }).always(function() {
                if (!redir && (send_but !== null)) {
                    if (ajax_content) send_but.trigger("finish_content.search");
                    else send_but.trigger("finish.search");
                }
            });
    };

    var respProc = function (msg) {

        if (ajax_content) {
            if (ajax_content_pushstate) window.history.pushState({path: form_act},'', form_act);
            send_but.html(msg);

            if ((callback_func !== null) && (typeof(callback_func === 'function'))) callback_func(msg, true);

            if ((el_clicked !== null) && (el_clicked.data('left-menu'))) {
                $('#side-menu li a').removeClass('active');
                el_clicked.addClass('active');
            }

            tooltip();
            return false;
        }

        if ((callback_func !== null) && (typeof(callback_func === 'function'))) {
            callback_func(msg, true);
            return false;
        }

        redir = false;


        if (type_data == 'JSON') {
        if(parseInt(msg.status)==1) {
            redir = true;
            window.location.href = msg.msg;
        } else if (msg.status == 2) {
            alert_bl.html(msg.msg);
            alert_bl.show();
        } else if (msg.status == 3) {
            var func = msg.msg[0].name;
            window[func](form, msg.msg);
        } else if (msg.status == 4) {
            alert(msg.msg);
            //showModal(msg.msg.title, msg.msg.content);
        } else if (msg.status == 5) {
            redir = true;
            alert_bl.html(msg.msg.msg);
            alert_bl.show();
            window.location.href = msg.msg.url;
        } else if (msg.status == 6) {
            $.notify({
                icon: 'fa fa-warning fa-fw',
                message: msg.msg
            },{
                type: 'success',
                delay: 4000,
                animate: {
                    exit: 'animated fadeOut'
                }
            });

            if (redirect_after_success) {
                var ajax_req = new ajaxReq();
                ajax_req.ajaxContent(redirect_after_success);
            }

            if (func_after_success) {
                window[func_after_success](form, msg);
            }

            if (class_after_success) {
                procEventClass(class_after_success, form, msg);
            }

        } else if (msg.status == 7) {
            $.notify({
                icon: 'fa fa-times-circle fa-fw',
                message: msg.msg
            },{
                type: 'danger',
                delay: 4000,
                animate: {
                    exit: 'animated fadeOut'
                }
            });
        } else {
            if (form.find('.form-group-m').length) {
                var form_cl = 'form-group-m';
            } else {
                var form_cl = 'form-group';
            }

            $.each(msg.msg, function(i, val){
                form.find('.'+form_cl+'[data-name="'+val.name+'"]').addClass('has-error');
                if (val.error != '') {
                    //alert_bl.html(val.error);
                    //alert_bl.show();
                    form.find('[name="'+val.name+'"]').popover({content : val.error, placement: 'top', trigger: 'focus'});
                }
                        
            });  
            form.find('*').popover('show');
        }
        }

        if (func_after) {
            window[func_after](form, msg);
        }

        if (class_after) {
            procEventClass(class_after, form, msg);
        }

        if (redirect_after) {
            var ajax_req = new ajaxReq();
            ajax_req.ajaxContent(redirect_after);
        }
    };

    var delAlertClass = function() {
        alert_bl.removeClass();
        alert_bl.addClass('alert');
        alert_bl.addClass('alert-ajax');
    };

    var showPopover = function () {
        form.find('*').not($(this)).popover('hide');
        $(this).popover('show');
    };    

    var hidePopover = function () {
        form.find('*').popover('hide');
    };    

    var hideProperErr = function () {
        form.find('.add').val('');
        form.find('*').popover('destroy');
        form.find(".form-group").removeClass('has-error');
        form.find(".form-group-m").removeClass('has-error');
    };

    self.setData = function (el) {
        var act_sub = true;

        var conf_msg = el.data('confirm');
        if (conf_msg) {
            act_sub = confirm(conf_msg);
        }

        if (act_sub) {
            data_send = null;

            send_but = el;

            type_data = el.data('type-data') || type_data;
            
            var form_id = el.data('form-id');
            if (form_id) {
                if (form_id == 'prev') form = el.prev('form');
                else form = $('#'+form_id);
            } else form = el.closest('form');
            
            form_act = form.attr('action');
            alert_bl = form.find('.alert-ajax');

            func_after = el.data('func-after') || null;
            class_after = el.data('class-after') || null;

            redirect_after = el.data('redirect') || null;
            redirect_after_success = el.data('redirect-success') || null;

            func_after_success = el.data('func-success') || null;
            class_after_success = el.data('class-success') || null;

            hideProperErr();

            var ajsend = el.data('ajsend');
            if (ajsend != 'off') mch_ajsend(el);

            var func_call = el.data('func');
            if (func_call) window[func_call](form);

            var class_call = el.data('class');
            if (class_call) procEventClass(class_call, form);

            var error = el.data('error');
            if (error == 0) error_ajax = 0;
            else error_ajax = 1;

            //form.submit();
            send(true);
        }

        return false;
    };

    self.ajaxContent = function (el, data) {
        var data = data || {};
        callback_func = data.callback_func || null;

        if (typeof data.ajax_content_pushstate != 'undefined') {
            ajax_content_pushstate = data.ajax_content_pushstate;
        }

        type_req = "GET";
        type_data = "HTML";
        process_data = undefined;
        content_type = undefined;
        form_data = { type : 'ajax_content' };
        //form_data = 'type=ajax_content';
        
        
        if (el.jquery) {
            el_clicked = el;
            
            form_act = el.data('url') || el.attr('href');
        } else {
            form_act = el;
        }

        send_but = $('#page-container');
        ajax_content = true;
        mch_ajsend(send_but);

        send(true);

        // close left menu for mobile devices
        var navbar = $('#navbar-collapse');
        if (navbar.hasClass('collapse')) $('.navbar-collapse').collapse('hide');

        return false;
    };

    self.manReq = function (data, redirectOnFail = true) {
        form_act = data.form_act || null;
        type_req = data.type_req || type_req;
        type_data = data.type_data || type_data;
        form_data = data.form_data || form_data;
        ajax_indicator = data.ajax_indic || null;

        if (ajax_indicator) {
            send_but = ajax_indicator
            mch_ajsend(send_but);
        }

        if ((data.process_data == true) || (data.process_data == undefined)) {
            process_data = data.process_data;
        } else process_data = process_data;

        if ((data.content_type == true) || (data.content_type == undefined)) {
            content_type = data.content_type;
        } else content_type = content_type;

        callback_func = data.callback_func || null;
        //if ((data.callback_func !== null) && (typeof(data.callback_func === 'function'))) {
            //callback_func = data.callback_func;
        //};
        send(redirectOnFail);
    };

    var constructor = function () {
    };
    constructor();
};
function ajaxEvent() {
    $('body').on("click", ".send-req-form", function(e){
        var ajax_req = new ajaxReq();
        return ajax_req.setData($(this));
    }); 

    $('body').on("change", ".send-req-form-select", function(e){
        var ajax_req = new ajaxReq();
        return ajax_req.setData($(this)); 
    }); 

    $('body').on("click", ".ajax-content", function(e){
        e.preventDefault();
        var ajax_req = new ajaxReq();
        ajax_req.ajaxContent($(this));
    }); 

    $('body').on("click", ".click-event", function(e){
        if ($(this).is('a'))e.preventDefault();
        procEvent($(this));
    }); 

    $('body').on("change", ".change-event", function(e){
        e.preventDefault();
        procEvent($(this));
    }); 
}

function procEvent(el) {
    var func = el.data('function');
    if (func) {
        window[func](el);
    } else {
        var class_c = el.data('class');
        procEventClass(class_c, el);
    }
}

function procEventClass(class_c, el, datas) {
    var el = el || null;
    var datas = datas || null;
    var class_tmp = '';
    class_c = class_c.split('.');

    $.each(class_c, function(i, val){
        if (i == 0) {
            class_tmp = new window[val](el);
            if (datas) class_tmp.setData(datas);
        } else {
            var start_pos = val.indexOf('(') + 1;
            var end_pos = val.indexOf(')',start_pos);
            var args = val.substring(start_pos,end_pos);

            val = val.substring(0, start_pos - 1);

            if (args) {
                args = args.split(',');
                if (args.length == 1) {
                    class_tmp[val](args[0]);
                } else {
                    class_tmp[val](args);
                }
            } else {
                class_tmp[val]();
            }
        }
    });
}

function mch_ajsend(el) {

    if (el.is('div')) {
        el.on("start_content.search", function() {
            el.addClass('page-container-loading');
        });
        el.on("finish_content.search", function() {
            el.removeClass('page-container-loading');
        });
    } else {
        el.on("start.search", function() {
            if (el.is('select')) {
                el.addClass('select-ajax');
            } else if (el.is('tr')) {
                var el_tmp = el.find('.glyphicon-refresh');
                if (!el_tmp.length) {
                    el.find('td:first').append('<span class="glyphicon glyphicon-refresh spinning"></span>');
                } else el_tmp.show();
            } else {
                el.button('loading');
            }
        });
        el.on("finish.search", function() {
            if (el.is('select')) {
                el.removeClass('select-ajax');
            } else if (el.is('tr')) {
                el.find('.glyphicon-refresh').hide();

            } else {
                if (el.data('complete-text')) {
                    el.button('complete');
                } else {
                    el.button('reset');
                }
            }
        });
    }
}
