$(function() {

    $('body').on("click", "#report-incorrect-data", function(e){
        e.preventDefault();
        var self = $(this);
        mch_ajsend(self);

        var z1ws6wwb1p41rs9;
        var src = ('https:' == location.protocol) ? 'https://' : 'http://';
        src += 'www.wufoo.com/scripts/embed/form.js';

        var options = {
            'userName':'bluecherry',
            'formHash':'z1ws6wwb1p41rs9',
            'autoResize':true,
            'height':'612',
            'async':true,
            'host':'wufoo.com',
            'header':'show',
            'ssl':true
        };
        
        self.trigger("start.search");
        $.getScript(src).done(function( script, textStatus, jqxhr ) {

            if (jqxhr.status == 200) {
                try { 
                    z1ws6wwb1p41rs9 = new WufooForm();
                    z1ws6wwb1p41rs9.initialize(options);
                    z1ws6wwb1p41rs9.display(); 
                    $('#wufoo-z1ws6wwb1p41rs9').show();
                } catch (e) {}

            }

            self.trigger("finish.search");
            
        }).fail(function( jqxhr, settings, exception ) {
            self.trigger("finish.search");
        });        
    });
    
});

var cameraProc = function (form) {
    var self = this;
    var els = {};

    var getEl = function(name, enabled, show, value) {
        show = show || false;
        enabled = enabled || false;
        value = value || null;
        var res = {};

        res.el = form.find('input[name="'+name+'"]');
        res.par = res.el.closest('div.form-group');

        if (enabled) res.el.prop('disabled', false);
        if (show) res.par.show();
        if (value != null) res.el.val(value);

        return res;
    };

    var showInps = function (show) {
        var inps = form.find('input[type="text"], input[type="password"]');
        var par_inps = inps.closest('div.form-group');
        inps.val('');

        if (show) {
            inps.prop('disabled', false);
            par_inps.show();
        } else {
            inps.prop('disabled', true);
            par_inps.hide();
        }
    };

    self.resetForm = function(enabled) {
        enabled = enabled || false;
        showInps(false);

        els.adv_bl.hide();
        
        getEl('camName', enabled, true);
        getEl('ipAddr', enabled, true);
        getEl('user', enabled, true);
        getEl('pass', enabled, true);
	getEl('substream', enabled, true);
    };

    self.setData = function (data) {
        getEl('mjpeg', true, true, data.mjpegPath);
        getEl('rtsp', true, true, data.rtspPath);
        getEl('port', true, true, data.rtspPort);
        getEl('portMjpeg', true, true, data.mjpegPort);
        getEl('user', true, true, data.user);
        getEl('pass', true, true, data.pass);
        getEl('onvif_port', true, true, '80');
	getEl('substream', true, true, data.substream);

        getEl('camName', true, true, '');
        getEl('ipAddr', true, true, '');

        if (data.rtspPath != '') self.blRtspMjpeg('rtsp');
        else {
            if (data.mjpegPath != '') self.blRtspMjpeg('mjpeg');
            else self.blRtspMjpeg('rtsp');
        }
    };

    self.blRtspMjpeg = function (type) {
        if (type == 'rtsp') {
            els.protocol_bl.val('IP-RTSP');
            els.rtsp_bl.show();
            els.mjpeg_bl.hide();
        } else {
            els.protocol_bl.val('IP-MJPEG');
            els.rtsp_bl.hide();
            els.mjpeg_bl.show();
        }

        els.adv_bl.show();

        getEl('rtsp', true, true, '');
        getEl('port', true, true, '554');
        getEl('mjpeg', true, true, '');
        getEl('portMjpeg', true, true, '80');
        getEl('onvif_port', true, true, '80');
    }

    var constructor = function () {
        els.adv_bl = $('#edittip-advanced-settings-block');
        els.rtsp_bl = form.find('.devices-rtsp-setting');
        els.mjpeg_bl = form.find('.devices-mjpeg-setting');
        els.protocol_bl = $('#devices-camera-protocol');
    };
    constructor();
};

function addIpCheckOnvifPort(el) {
    var form = el.closest('form');
    var ip_addr = form.find('input[name="ipAddr"]').val();
    var port = form.find('input[name="onvif_port"]').val();
    var user = form.find('input[name="user"]').val();
    var pass = form.find('input[name="pass"]').val();

    var ajax_req = new ajaxReq();
    ajax_req.manReq({
        form_act : '/addip/check-onvif-port',
        form_data : {
            ip_addr : ip_addr,
            port : port,
	    user : user,
	    pass : pass
        },
        ajax_indic : el,
        callback_func: function (msg, done) {
            if (done) {
                var cp = new cameraProc(el.closest('form'));
                cp.setData(msg.data);
            } else {

            }
        }
    });
}

function cameraChooseModel(el) {
    var ajax_req = new ajaxReq();
    ajax_req.manReq({
        form_act : '/ajax/addip.php?m=ops&model='+el.find('option:selected').val(),
        type_data : 'JSON',
        ajax_indic: el,
        callback_func: function (msg, done) {
            if (done) {
                var cp = new cameraProc(el.closest('form'));
                cp.setData(msg.data);
            } else {

            }
        }
    });
}

function cameraChooseManuf(el) {
    var sel_bl = $('#addip-model-select');
    var sel_val = el.find('option:selected').val();
    var par = sel_bl.closest('div.form-group');
    par.hide();

    var ajax_req = new ajaxReq();
    ajax_req.manReq({
        form_act : '/ajax/addip.php?m=model&manufacturer='+sel_val,
        type_data : 'HTML',
        ajax_indic: el,
        callback_func: function (msg, done) {
            if (done) {
                sel_bl.html(msg);
                
                var cp = new cameraProc(el.closest('form'));
                if ((sel_val != 'Generic') && (parseInt(sel_val) != 1)) {
                    cp.resetForm();
                    par.show();
                } else {
                    cp.resetForm(true);
                    cp.blRtspMjpeg('rtsp');
                }
            } else {

            }
        }
    });
}

