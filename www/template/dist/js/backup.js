$(function() {

});


var backupRestore = function (el) {
    var self = this;
    var els = {};

    self.showConfirm = function(act) {
        if (act == 1) {
            els.form_upload.hide();
            els.form_confirm.show();
        } else {
            els.form_upload.show();
            els.form_confirm.hide();
        }
    }

    self.resetForms = function () {
        els.form_upload.get(0).reset();
        els.form_confirm.get(0).reset();
    }

    self.setData = function (data) {
    }

    var constructor = function () {
        els.form_upload = $('#backup-form-upload');
        els.form_confirm = $('#backup-form-confirm');
    };
    constructor();
}

function backupCopyPass(form) {
    var pass = $('#backup-form').find('input[name="pwd"]').val();
    form.find('input[name="pwd"]').val(pass);
}
