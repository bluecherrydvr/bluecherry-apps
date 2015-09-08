$(function() {

});

var cameraProc = function (form) {
    var self = this;

    var getEl = function(name) {
        var res = {};
        res.el = form.find('input[name="'+name+'"]');
        res.par = res.el.closest('div.form-group');

        return res;
    };

    self.resetForm = function() {
        getEl('ipAddr').par.hide();
    };

    var constructor = function () {
    };
    constructor();
};

function cameraChooseModel(el) {
    var ajax_req = new ajaxReq();
    ajax_req.manReq({
        form_act : '/ajax/addip.php?m=ops&model='+el.find('option:selected').val(),
        type_data : 'JSON',
        callback_func: function (msg, done) {
            if (done) {
                alert(msg.data.camName);
            } else {

            }
        }
    });
}

function cameraChooseManuf(el) {
    var sel_bl = $('#addip-model-select');
    var par = sel_bl.closest('div.form-group');
    par.hide();

    var ajax_req = new ajaxReq();
    ajax_req.manReq({
        form_act : '/ajax/addip.php?m=model&manufacturer='+el.find('option:selected').val(),
        type_data : 'HTML',
        callback_func: function (msg, done) {
            if (done) {
                sel_bl.html(msg);
                par.show();
                var cp = new cameraProc(el.closest('form'));
                cp.resetForm();
            } else {

            }
        }
    });
}

