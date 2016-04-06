
<div class="row">
    <div class="col-lg-12">
        <h1 class="page-header"></h1>
    </div>
</div>

<div class="row">
    <div class="col-lg-12 col-md-12">
    <?php
        if (!$version->version['up_to_date']) {
            echo NOT_UP_TO_DATE_LONG.'<BR>'.NOT_UP_TO_DATE_LINK.'<hr />';
        }
    ?>

    <?php
        echo INSTALLED.": <b>{$version->version['installed']}</b> <br />".CURR.": <b>{$version->version['current']}</b>";
    ?>

    </div>
</div>
