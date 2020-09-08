
$('#btn-webhook-delete').click(function (event) {
    event.preventDefault();

    if (!confirm('Are you sure you want to delete webhook?')) {
        return false;
    }

    var t = $(this);

    $.post(t.val(), function (result) {

        if (result.status === 7) {
            alert(result.msg);
            return;
        }

        window.location.href = '/webhook';

    }, 'json');

    return false;
});

$('#webhook-form').submit(function (event) {
    event.preventDefault();

    var t = $(this);

    $.post(t.prop('action'), t.serialize(), function (result) {

        if (result.status === 7) {
            alert(result.msg);
            return;
        }

        window.location.reload();
    }, 'json');

    return false;
});
