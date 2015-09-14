$(function() {
});


function notifEditRule(el) {
    var par = el.closest('.notif-rule-item');
        
    var ajax_req = new ajaxReq();
    ajax_req.manReq({
            form_act : '/ajax/notifications.php?mode=getedit&id='+par.data('id'),
            type_data : 'HTML',
            ajax_indic : el,
            callback_func: function (msg, done) {
                if (done) {
                    par.html(msg);
                } else {

                }
            }
        }); 
}

function notifDelRule(el) {
    var par = el.closest('.notif-rule-item');
        
    var ajax_req = new ajaxReq();
    ajax_req.manReq({
            form_act : '/ajax/notifications.php?mode=delete&id='+par.data('id'),
            ajax_indic : el,
            callback_func: function (msg, done) {
                if (done) {
                    par.closest('div').remove();
                } else {

                }
            }
        }); 
}

function notifChangeStatus(el) {
    var par = el.closest('.notif-rule-item');

    var ajax_req = new ajaxReq();
    ajax_req.manReq({
            form_act : '/ajax/notifications.php?mode=changeStatus&id='+par.data('id'),
            ajax_indic : el,
            callback_func: function (msg, done) {
                if (done) {
                    var table = par.find('table');
                    if (table.hasClass('table-disabled')) {
                        table.removeClass('table-disabled');

                        el.data('complete-text', $('#notif-status-disabled').html());
                    } else {
                        table.addClass('table-disabled');

                        el.data('complete-text', $('#notif-status-enabled').html());
                    }
                } else {

                }
            }
        }); 
}
