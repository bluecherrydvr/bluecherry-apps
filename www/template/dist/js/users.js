$(function() {
    
    usersEmailsAddDel();
});

function usersAfterAdd(form, msg) {
    if (msg.status == 6) {
        var ajax_req = new ajaxReq();
        ajax_req.ajaxContent('/ajax/users.php?id='+msg.data);
    }
}

function usersEmailsAddDel() {
    $('body').on("click", ".users-add-email", function(e){
        e.preventDefault();
        var el = $('.users-add-email-inp:first').clone();
        el.find('input').val('');
        el.addClass('input-group');
        el.find('.input-group-btn').css('display', 'table-cell');
        el.appendTo('#users-add-email-inp-bl');
    });
    
    $('body').on("click", ".users-del-email", function(e){
        e.preventDefault();
        var par = $(this).closest('.users-add-email-inp');
        var count = $('.users-add-email-inp').length;
        if (count > 1) {
            par.remove();
        } 
        
        if (count == 2) {
            $('.users-add-email-inp .input-group-btn').hide();
            $('.users-add-email-inp').removeClass('input-group');
        }

    });
}

