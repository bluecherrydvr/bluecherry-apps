$(function() {
});


function profileSaved(form, msg) {
    form.find('input[name="password_current"], input[name="password_new"]').val('');
}
