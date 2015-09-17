$(function() {

});

function backupCopyPass(form) {
    var pass = $('#backup-form').find('input[name="pwd"]').val();
    form.find('input[name="pwd"]').val(pass);
}
