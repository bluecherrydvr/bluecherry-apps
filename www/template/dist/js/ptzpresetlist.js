$(function() {

    
});

function ptzPreSetListSelect(el) {
    var ajax_req = new ajaxReq();
    ajax_req.manReq({
        form_act : '/ajax/update.php',
        type_data : 'JSON',
        form_data: { id : el.data('id'), mode : 'update', ptz_control_path : el.data('path'), ptz_control_protocol : 'PELCO' , type : 'Devices' },
        ajax_indic: el,
        callback_func: function (msg, done) {
            if (done) {
                var par = el.closest('table');
                par.find('tr').removeClass('active');
                el.addClass('active');
            } else {

            }
        }
    });

}
