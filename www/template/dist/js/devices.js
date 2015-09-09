$(function() {

    
    $('body').on("click", "#edittip-advanced-settings", function(e){
        var el = $('#edittip-advanced-settings-block');
        if (el.is(':visible')) el.hide();
        else el.show();
    });

});

function getMotionMap(form) {
    var tds = form.find('.table-grid td');
    var map = '';
    $.each(tds, function(i, val){
        var type = $(this).attr('data-type');
        map += type;
    });

    if (map) {
        $('#motion-map').val(map);
    }
        //$('#motion-map').val('map');
}

var motionGrid = function(el) {
    var self = this;
    var grid_bl = null;
    var table_grid = null;
    var table_grid_tds = null;
    var grid_color = null;
    var color_array = { 'bg-default' : 0 , 'bg-success' : 1, 'bg-info' : 2, 'bg-primary' : 3, 'bg-warning' : 4, 'bg-danger' : 5};

    var gridChanges = function (el_tmp) {
        el_tmp = el_tmp || table_grid_tds;

        el_tmp.removeClass('bg-danger');
        el_tmp.removeClass('bg-warning');
        el_tmp.removeClass('bg-primary');
        el_tmp.removeClass('bg-info');
        el_tmp.removeClass('bg-success');
        el_tmp.removeClass('bg-default');

        el_tmp.addClass(grid_color);

        el_tmp.attr('data-type', color_array[grid_color]);
    };

    var btnChanges = function () {
        var btns = $('.motion-btn-sens');
        btns.addClass('disabled');
        el.removeClass('disabled');
    }

    var getGridColor = function () {
        var btn = $('.motion-sens-bl').find('button:not(.disabled)');
        grid_color = 'bg-'+btn.data('active');
    }

    self.getGridColorP = function () {
        getGridColor();
    };

    self.gridChangesP = function (el_tmp) {
        gridChanges(el_tmp);
    }

    self.off = function () {
        grid_color = 'bg-default';
        el.data('active', 'default');
        btnChanges();
    };

    self.minimal = function () {
        grid_color = 'bg-success';
        el.data('active', 'success');
        btnChanges();
    };

    self.low = function () {
        grid_color = 'bg-info';
        el.data('active', 'info');
        btnChanges();
    };

    self.average = function () {
        grid_color = 'bg-primary';
        el.data('active', 'primary');
        btnChanges();
    };

    self.high = function () {
        grid_color = 'bg-warning';
        el.data('active', 'warning');
        btnChanges();
    };

    self.veryHigh = function () {
        grid_color = 'bg-danger';
        el.data('active', 'danger');
        btnChanges();
    };

    self.showHideImage = function () {
        var img = grid_bl.find('img');
        if (img.is(':visible')) img.hide();
        else img.show();
    };
    
    self.clearAll = function () {
        grid_color = 'bg-default';
        gridChanges();
    };
    
    self.fillAll = function () {
        getGridColor();
        gridChanges();
    };

    self.drawGrid = function () {
        var img = grid_bl.find('img');

        var width = img.width();
        var height = img.height();
    	var numcols = width/16;
        var numrows = height/16;

        var grid_table = '<table class="table-grid" border="1" style="width: '+width+'px; height: '+height+'px;">';

    	for (row = 1; row<=numrows; row++) {
            grid_table += '<tr>';
            for(col = 1; col<=numcols; col++) {
                grid_table += '<td class="bg-primary">';
                grid_table += '</td>';
            }
            grid_table += '</tr>';
        };

        grid_table += '<table>';

        grid_bl.append(grid_table);
        grid_bl.find('table td').attr('data-type', 3);


        $('body').on("mousedown", ".table-grid td", function(e){
            var mg = new motionGrid(null);
            mg.getGridColorP();

            $('body').on("mousemove", ".table-grid td", function(e){
                mg.gridChangesP($(this));

            }).on('mouseup', function(e) {
                $('body').off("mousemove", ".table-grid td");
            });
        });

        $('body').on("click", ".table-grid td", function(e){
            var mg = new motionGrid(null);
            mg.getGridColorP();
            mg.gridChangesP($(this));
        });
    };

    var constructor = function () {
        grid_bl = $('.grid-bl');
        table_grid = $('.table-grid');
        table_grid_tds = $('.table-grid td');
    };
    constructor();
}

function delCamera(form) {
    form.closest('.well').remove();
}

function deviceSwith(form, msg) {
    // okay
    if (msg.status == 6) {
        var par = form.closest('.well');
        var par_col = par.parent();
        var icon = par.find('.devices-device-name i');
        var submit = par.find('button.devices-onoff-form');
        var disabled_bl = $('#devices-ip-disabled');
        var ok_bl = $('#devices-ip-ok');

        if (icon.hasClass('text-status-OK')) {
            icon.removeClass('text-status-OK');
            icon.addClass('text-status-disabled');
            submit.data('complete-text', $('#devices-text-status-disabled').text());

            par_col.detach().appendTo(disabled_bl);

            disabled_bl.show();
        } else {
            icon.removeClass('text-status-disabled');
            icon.addClass('text-status-OK');
            submit.data('complete-text', $('#devices-text-status-OK').text());

            par_col.detach().appendTo(ok_bl);

            if (disabled_bl.find('div').length < 1) disabled_bl.hide();
        }

    } else {
        // error

    }
}
