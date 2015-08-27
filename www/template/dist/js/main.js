$(function() {

    commons();
    ajaxEvent();
});

function commons() {

    $('body').on("click", ".downloadClient", function(e){
        e.preventDefault();
        window.open(var_rm_client_download);
    });
    
    $('body').on("click", ".liveView", function(e){
        e.preventDefault();
        window.location.href = '/?l=true';
    });
}


var ajaxReq = function () {
    var self = this;
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
    var type_req = "POST";
    var type_data = "JSON"

    var send = function () {
        if (form !== null) form_data =  new FormData(form[0]);
        
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
                    send_but.trigger("start.search");
                }
            })
            .done(function(msg) {
                respProc(msg);
            })
            .fail(function(msg) {
                if (error_ajax) alert('err ajax');
            }).always(function() {
                if (!redir) send_but.trigger("finish.search");
            });
    };

    var respProc = function (msg) {

        if (ajax_content) {
            //window.history.pushState({},'', form_act);
            send_but.html(msg);
            return false;
        }

        redir = false;
        alert_bl.hide();


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
            alert_bl.html(msg.msg);
            alert_bl.show();
        } else if (msg.status == 7) {
            alert_bl.html(msg.msg);
            alert_bl.show();
            form.find('input').not('input[name="_token"]').val('');
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
            form = el.closest('form');
            form_act = form.attr('action');
            alert_bl = form.find('.alert');

            hideProperErr();

            var ajsend = el.data('ajsend');
            if (ajsend != 'off') mch_ajsend(el);

            var func_call = el.data('func');
            if (func_call) window[func_call](form);

            var error = el.data('error');
            if (error == 0) error_ajax = 0;
            else error_ajax = 1;

            //form.submit();
            send();
        }

        return false;
    };

    self.ajaxContent = function (el) {
        type_req = "POST";
        type_data = "HTML";
        process_data = undefined;
        content_type = undefined;
        form_data = { type : 'ajax_content' };
        //form_data = 'type=ajax_content';
        form_act = el.attr('href');
        send_but = $('#page-container');
        ajax_content = true;
        mch_ajsend(send_but);

        send();

        // close left menu for mobile devices
        var navbar = $('#navbar-collapse');
        if (navbar.hasClass('collapse')) $('.navbar-collapse').collapse('hide');

        return false;
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

    $('body').on("click", ".ajax-content", function(e){
        e.preventDefault();
        var ajax_req = new ajaxReq();
        ajax_req.ajaxContent($(this));
    }); 
}


function mch_ajsend(el) {
    el.on("start.search", function() {
        if (el.is('div')) {
            el.addClass('page-container-loading');
        } else {
            el.button('loading');
        }
    });
    el.on("finish.search", function() {
        if (el.is('div')) {
            el.removeClass('page-container-loading');
        } else {
            el.button('reset');
        }
    });
}
