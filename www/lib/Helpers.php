<?php

//makes an options dropdown
function arrayToSelect($options, $selected='', $id='', $class='', $disabled=false, $data_attr = ''){
	$tmp = "<select id='{$id}' class='{$class} form-control' {$data_attr} name='{$id}'".(($disabled) ? ' disabled="disabled"' : '').">";
	$assoc = is_assoc($options);
	foreach($options as $i => $value){
		if (is_array($value))$value = current($value);
		$tmp .= "<option value='".(($assoc) ? $i : $value)."'".(($value==$selected || ($assoc && $i==$selected)) ? ' selected="selected"' : '').">$value</option>";
	}
	$tmp .= "</select>";
	return $tmp;
}

function addJs($val) {
    $varpub = VarPub::get();
    if (!is_array($varpub->javascript)) {
        $varpub->javascript = Array();
    }
    $js = $varpub->javascript;

    $js[] = $val;

    $varpub->javascript = $js;
}

function getJs() {
    $res = '<script type="text/javascript">';

    $varpub = VarPub::get();
    if ($varpub->javascript) {
        foreach ($varpub->javascript as $val) {
            $res .= $val;
        }
    }

    $res .= '</script>';

    return $res;
}


