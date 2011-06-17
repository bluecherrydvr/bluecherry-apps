var loginPage = new Class({
	initialize: function(){
		$('login').focus();
		if ($('login-message').get('html')!=''){
			$('login-message').fade('in');
		}
		$('login-button').addEvent('click', function(ev){
			ev.stop();
			$('login-form').set('send', {
				onComplete: function(text){
					message = new loginPageMessage(text);
				}
			});
			$('login-form').send();
		});
	}
});

var loginPageMessage = new Class({
	initialize: function(message){
		if (message=='OK'){
			location.reload();
			return false;
		}
		$('login-message').set('html', message);
		$('login-message').fade('in');
	}
})

window.addEvent('domready', function(){
	init = new loginPage();
});
