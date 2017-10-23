<?php defined('INDVR') or exit();

/*
    Copyright (c) 2010-2011 Bluecherry, LLC.
    http://www.bluecherrydvr.com / support@bluecherrydvr.com
    Confidential, all rights reserved. No distribution is permitted.
 */

#company global
define('DVR_COMPANY_NAME', 'RCL');
define('DVR_DVR', 'DVR');

#headers
define('PAGE_HEADER_MAIN', 'Administracao');
define('PAGE_HEADER_VIEWER', 'AO VIVO');
define('PAGE_HEADER_SCHED', 'Acesso Nao Disponivel Neste Momento');

define('PAGE_HEADER_LOGIN', 'Logar');
define('PAGE_HEADER_SEPARATOR', ' :: ');

#main menu
define('MMENU_NEWS', 'Anuncios');
define('MMENU_GS', 'Configuracoes Gerais');
define('MMENU_STORAGE', 'Armazenamento');
define('MMENU_USERS', 'Usuarios');
define('MMENU_LOG', 'Eventos do Sistema');
define('MMENU_DEVICES', 'Dispositivos');
define('MMENU_GPIO', 'GPIO');
define('MMENU_SCHED', 'Agendamento Global');
define('MMENU_ALRMSCHED', 'Programação de alarmes');
define('MMENU_LIVEVIEW', 'Ao Vivo');
define('MMENU_PLAYBACK', 'Reproducao');
define('MMENU_STATISTICS', 'Estatisticas de Eventos');
define('MMENU_ACTIVE_USERS', 'Usuarios Conectados');
define('MMENU_CLIENT_DOWNLOAD', 'Download  Cliente Remoto');
define('MMENU_NOTFICATIONS', 'Notificacao por Email');
define('MMENU_BACKUP', 'Backup de Configuracao');
define('MMENU_LICENSING', 'Chaves de Acesso');
define('MMENU_AUDIT_LOG', 'Log de auditoria');

#GPIO
define('GPIO', 'GPIO');
define('GPIO_RESET_RELAYS', 'Resetar Relés');
define('GPIO_STATE', 'Estado GPIO');
define('GPIO_RELAYS', 'Relés');
define('GPIO_SENSORS', 'Sensores');
define('GPIO_CARD', 'Placa ');
define('GPIO_SENSORS_TO_RELAYS_MAP', 'Mapeamento de sensores para relés');
define('GPIO_SENSORS_ON_CARD', 'Sensores da Placa ');
define('GPIO_RELAY', 'Relé ');
define('GPIO_ON_CARD', ' da Placa ');
define('GPIO_SENSORS_TO_CAMERAS_MAP', 'Sensores para mapeamento de câmeras');



#main
define('COUND_NOT_OPEN_PAGE', 'Pagina Nao Pode Carregar');
define('COUND_NOT_OPEN_PAGE_EX', 'Pagina nao pode carregar. Ceritifique-se que voce esta habilitado a conectar no DVR.');

#dsched
define('DSCED_HEADER', 'Acesso as Imagens ao Vivo Nao Permitido.');
define('DSCED_MSG', 'Nao pode usar a Interface Web, contate o administrador do sistema.');


#die (fatal) messages
define('LANG_DIE_COULDNOTCONNECT', 'Erro Fatal: nao pode conectar ao banco de dados: ');
define('LANG_DIE_COULDNOTOPENCONF', 'Erro Fatal: nao pode ler arquivos de configuracao.');
define('LANG_GET', 'Buscar');
define('LANG_FINDCAMERAS', 'Procurar Cameras');
define('LANG_ADDSELECTEDCAMERAS', 'Adicionar Cameras');


#general
define('BB_MAP_ARROW', '>');
define('SAVE_SETTINGS', 'Salvar Configuracoes');
define('SAVE_CHANGES', 'Salvar Alteracoes');
define('SAVE', 'Salvar');
define('SAVE_AS', 'Salvar Como...');
define('CLEAR_ALL', 'Apagar Tudo');

define('CHANGES_OK', 'Alteracoes salvas com sucesso');
define('CHANGES_FAIL', 'Modificacoes nao disponiveis no momento , por favor tente mais tarde.'); 

define('ENABLE_DEVICE_NOTENOUGHCAP', '<b>A Placa nao tem capacidade suficiente para acomodar este dispositivo. <br/></b>Reduza a resolucao / taxa de quadros de outros dispositivos neste cartao.');
define('NEW_DEV_NEFPS', 'O dispositivo foi configurado usando a configuracao generica, mas nao ha capacidade de Placa Captura suficiente para habilita-la. Reduza o Framerate/Resolucao de outros dispositivos ja configurados e tente novamente.');

#general settings
define('GLOBAL_SETTINGS', 'DVR Configuracoes Gerais');
define('G_DISABLE_VERSION_CHECK', 'Checar Updates Disponiveis');
define('G_DISABLE_VERSION_CHECK_EX', 'Exige Servidor para Acesso a Internet');
define('G_DVR_NAME', 'Nome DVR');
define('G_DVR_NAME_EX', 'Voce pode nomear seu DVR apos sua localizacao ou uso');
define('G_DVR_EMAIL', 'Endereco de Email'); 
define('G_DVR_EMAIL_EX', 'Notificacao de Email');
define('G_DEV_NOTES', 'Notas');
define('G_DEV_NOTES_EX', 'DVR Memos');
define('G_DISABLE_IP_C_CHECK', 'Desativar a verificacao da conectividade da camera IP');
define('G_DISABLE_IP_C_CHECK_EX', 'Melhorar WEB UI Performance');
define('G_DISABLE_WEB_STATS', 'Desativar Exibicao de estatisticas na Interface do Usuário da WEB'); 
define('G_DISABLE_WEB_STATS_EX', 'Melhorar o Desempenho da Interface da WEB');
define('G_DATA_SOURCE', 'Origem da lista');
define('G_DATA_SOURCE_LOCAL', 'Lista Local');
define('G_DATA_SOURCE_LIVE', 'Ao Vivo (Voce precisa de uma Conexao Ativa com a Internet)');

define('G_SMTP_TITLE', 'Configuracao Servidor SMTP');
define('G_PERF_TITLE', 'Perfomance/Conectividade');

define('G_MAX_RECORD_TIME', 'Tamanho da Gravacao');
define('G_MAX_RECORD_TIME_EX', 'Dividir Videos em Segmentos de');

define('G_MAX_RECORD_AGE', 'Armazenamento Maximo');
define('G_MAX_RECORD_AGE_EX', 'Deixar o sistema remover todas as gravacoes mais antigas do que determinado numero de dias (opcional)');

define('G_VAAPI_DEVICE', 'Dispositivo VAAPI');
define('G_VAAPI_DEVICE_EX', 'Nó de renderização para aceleração de hw VAAPI de decodificação e codificação de vídeo');

#mail
define('G_SMTP_SERVICE', 'Servico de Email');
define('G_SMTP_EMAIL_FROM', 'Valor SMTP');
define('G_SMTP_USERNAME', 'Nome de Usuário');
define('G_SMTP_PASSWORD', 'Senha');
define('G_SMTP_HOST', 'Endereco Servidor SMTP');
define('G_SMTP_PORT', 'Porta SMTP');
define('G_SMTP_SSL', 'SSL');
define('G_SMTP_SERVICE_EX', '');
define('G_SMTP_EMAIL_FROM_EX', 'Quando o DVR RCL envia email, esse valor e usado como endereco do remetente ("From")');
define('G_SMTP_USERNAME_EX', '');
define('G_SMTP_PASSWORD_EX', '');
define('G_SMTP_HOST_EX', 'e.g mail.rclindustria.com.br');
define('G_SMTP_PORT_EX', '');
define('G_SMTP_SSL_EX', '');
define('G_SMTP_FAILED', 'Envio de email desativado com esta configuracoes.');

define('G_SMTP_DEF_MTA', 'Sistema Padrao MTA');
define('G_SMTP_SMTP', 'Usuario Configurado SMTP');
define('G_SEND_TEST_EMAIL_TO', 'Enviar Email Teste Para');
define('G_SEND_TEST_EMAIL', 'Enviar');
define('G_TEST_EMAIL_SUBJECT', 'Email de Teste');
define('G_TEST_EMAIL_BODY', 'Email de Teste');
define('G_TEST_EMAIL_SENT', 'Mensagem Enviada');

define('G_TOTAL_BANDWIDTH_LIMIT', 'Limitador de Banda');
define('G_GPIO_RELAY_RESET_TIMEOUT', 'GPIO Relays Reset Timeout');


#users page
define('EDIT_ACCESS_LIST', 'Permissao Acesso das Cameras');
define('USERS_ALL_TABLE_HEADER', 'Usuarios DVR');
define('USERS_DETAIL_TABLE_HEADER', 'Detalhes dos Usuarios : ');

define('CAMERA_PERMISSIONS_SUB', 'Permissao Acesso de Cameras dos Usuarios');
define('RESTRICT_ALL', 'Restringir Todas');
define('ALLOW_ALL', 'Permitir Todas');
define('CAMERA_PERMISSIONS_LIST_TITLE', 'Clique no Nome da Camera para Permitir/Bloquear Acesso');

define('USERS_ALL_TABLE_SUBHEADER', 'Usuarios Atualmente Cadastrados no  DVR.');
define('USERS_DETAIL_TABLE_SUBHEADER', 'Editar Informacoes do Usuario e Acesso. ');

define('USERS_TABLE_HEADER_NAME', 'Nome Completo');
define('USERS_TABLE_HEADER_LOGIN', 'Login');
define('USERS_TABLE_HEADER_EMAIL', 'Email');
define('USERS_TABLE_HEADER_STATUS', 'Status');
define('USERS_LIST', 'Usuarios do DVR');

define('DELETE_USER', 'Deletar Usuario');
define('USERS_EDITACCESSLIST', ' Editar Acesso');
define('DELETE_USER_SELF', 'Voce Nao Pode Remover seu Proprio Registro de Usuario');
define('USER_DELETED_OK', 'Registro do Usuario Excluido com Exito');

define('USERS_NEW', 'Criar Novo Usuario');
define('USERS_NAME', 'Nome');
define('USERS_NAME_EX', 'Nome completo');
define('USERS_LOGIN', 'Login');
define('USERS_LOGIN_EX', 'Login');
define('USERS_PASSWORD', 'Senha');
define('USERS_PASSWORD_EX', 'Senha');
define('USERS_PASSWORD_CURRENT', 'Senha Atual ');
define('USERS_PASSWORD_NEW', 'Nova Senha');
define('USERS_PASSWORD_WRONG', 'Senha Atual Incorreta.');
define('USERS_PHONE', 'Telefone');
define('USERS_PHONE_EX', 'Telefone #');
define('USERS_ACCESS_SETUP', 'Accesso Administrador');
define('USERS_ACCESS_SETUP_EX', 'Privilegios Administrativos');
define('USERS_ACCESS_REMOTE', 'Acesso Remoto');
define('USERS_ACCESS_REMOTE_EX', 'Acesso Cliente Remoto');
define('USERS_ACCESS_WEB', 'Acesso WEB');
define('USERS_ACCESS_WEB_EX', 'Acesso Visualizador WEB');
define('USERS_ACCESS_BACKUP', 'Arquivo de Video');
define('USERS_ACCESS_BACKUP_EX', 'Permitir Acesso a Eventos Anteriores');
define('USERS_EMAIL', 'Emails');
define('USERS_EMAIL_EX', 'Email para Notificacoes ');
define('EMAIL_LIMIT', 'Limite');
define('EMAIL_LIMIT_EX', 'Emails/Hora (0 - Ilimitado)');
define('USERS_NOTES', 'Notas do Usuário');
define('USERS_NOTES_EX', 'Visivel Apenas para Administradores');
define('USERS_CHANGEPASSWORD', 'Alterar senha no próximo login');
define('USERS_CHANGEPASSWORD_EX', 'Solicitar Alteracao de Senha');

define('USERS_STATUS_ADMIN', 'Administrador');
define('USERS_STATUS_VIEWER','Visualizador');
define('USERS_STATUS_DISABLE', 'Desativada');


define('USERS_STATUS_SETUP', 'Admininstrador');

#log
define('LOG_EMPTY', 'Arquivo de Log Atual esta Vazio.');
define('LOG_ALL', 'Registro Inteiro');
define('LOG_COULD_NOT_OPEN', 'Nao Pode Abrir Arquivo de Logs');
define('LOG_HOW_MANY', 'Linhas Inferiores Carregando...');
define('LOG_FILE_DOES_NOT_EXIST', 'Arquivo de Log nao Existe');
define('LOG_DOWNLOAD', 'Transferir Informacoes de Depuracao');

define('SCROLL_TO_BOTTOM', 'Ir Para o Final');
define('SCROLL_TO_TOP', 'Ir Para o Inicio');

#news
define('NEWS_HEADER', ' Anuncios');

#login
define('LOGIN_WRONG', 'Login ou Senha Incorreto, Tente Novamente.');
define('LOGIN_OK', 'Por Favor, Aguarde Enquanto Voce e Redirecionado...');
define('NA_WEB', 'Voce Nao Tem Permissao para Usar a Interface  WEB');
define('NA_CLIENT', 'Voce nao Tem Permissao Para usar o Cliente Remoto');

define('WARN_DEFAULT_PASSWORD', 'Voce Nao Alterou a Senha Padrao Neste Sistema. Para Seguranca Recomendamos Altamente <a id="updateAdminPassword" href="#">Atualizar sua Senha</a>.');

#reencode
define('REENCODE_HEADER', 'Configuracao Reencoder Ao Vivo');
define('REENCODE_SUBHEADER', 'Definicoes reencaminhamento ao vivo para a camera');
define('REENCODE_ENABLED', 'Habilitar reencoder ao vivo');
define('REENCODE_BITRATE', 'Taxa de bits de saida');
define('REENCODE_RESOLUTION', 'Resolucao de saida');
define('DEVICE_EDIT_REENCODE', 'Configuracoes de reencodificacao ao vivo');


#devices
define('CARD_HEADER', 'RCL Hardware De Compressao');
define('CARD_CHANGE_ENCODING', 'Clique Para Alterar a Codificacao');
define('LOCAL_DEVICES', 'Dispositivos Locais:');
define('PORT', 'Porta');
define('DEVICE_VIDEO_SOURCE', 'Fonte de Video');
define('DEVICE_VIDEO_STATUS', 'Status do Dispositivo');
define('DEVICE_VIDEO_STATUS_OK', 'Ativado');
define('DEVICE_VIDEO_STATUS_CHANGE_OK', 'Desativar');
define('DEVICE_VIDEO_STATUS_disabled', 'Desativado');
define('DEVICE_VIDEO_STATUS_CHANGE_disabled', 'Habilitar');
define('DEVICE_VIDEO_NAME_notconfigured', 'Nao Configurado');
define('DEVICE_VIDEO_STATUS_notconfigured', 'Desativado');
define('DEVICE_VIDEO_STATUS_CHANGE_notconfigured', 'Habilitar');
define('DEVICE_VIDEO_RESFPS', 'Resolucao do Dispositivo/FPS');
define('DEVICE_EDIT_MMAP', 'Configuracoes de Deteccao de Movimento');
define('DEVICE_EDIT_VIDADJ', 'Configuracoes Video/Audio');
define('DEVICE_EDIT_SCHED', 'Agendamento');
define('DEVICE_EDIT_ONCAM', 'Configuracao na Camera'); 
define('VIDEO_ADJ', 'Ajuste de Video');
define('DEVICE_UNNAMED', 'Sem Nome');
define('TOTAL_DEVICES', 'Dispositivos Totais');
define('ENABLE_ALL_PORTS', 'Habilitar Todas as Portas');

define('VA_HEADER', 'Ajustes de Video');
define('VA_BRIGHTNESS', 'Brilho');
define('VA_HUE', 'Matriz');
define('VA_AUDIO', 'Ganho de Audio');
define('VA_AUDIO_ENABLE', 'Habilitar Audio');
define('VA_SATURATION', 'Saturacao');
define('VA_CONTRAST', 'Contraste');
define('VA_VIDEO_QUALITY', 'Qualidade do Video');
define('VA_SETDEFAULT', 'Redefinir Valores para o Padrao');
define('VA_COMPRESSION', 'Compressão');
define('VA_COMPRESSION_LOW', 'Baixo');
define('VA_COMPRESSION_MEDIUM', 'Medio');
define('VA_COMPRESSION_HIGH', 'Alto');

define('SUB_ENABLE', 'Ativar Legendas');

define('DEVICE_ENCODING_UPDATED', '<![CDATA[A definicao padrao de video foi alterada. <hr /> <b>Para que a alteracao tenha efeito, voce precisa reiniciar o servidor.</b><br> Se estiver a mudar de NTSC para PALM, siga as instrucoes no <a href="http://help.rclindustria.com/spaces/manuals/manuals/version2/lessons/22732-Enabling-video-ports">esta pagina</a>]]>');
define('DB_FAIL_TRY_LATER', 'Nao foi possivel aplicar as alteracoes. Certifique-se de que consegue conectar ao servidor DVR e que o servidor esta com a aplicacao em funcionamento.');

define('MMAP_HEADER', 'Configuracoes de Deteccao de Movimento');
define('MMAP_SELECTOR_TITLE', 'Selecionar Nivel de Sensibilidade');
define('BUFFER_TITLE', 'Escolha Quantos Segundos para Gravar Antes e Depois dos Eventos de Movimento');
define('PRE_REC_BUFFER', 'Pre-Evento');
define('POST_REC_BUFFER', 'Pos-Evento');
define('MOTION_ALGORITHM_TITLE', 'Algoritmo de Deteccao de Movimento');
define('MOTION_ALGORITHM', 'Algoritmo ');
define('MOTION_DEFAULT', 'Padrao');
define('MOTION_EXPERIMENTAL', 'Experimental');
define('MOTION_FRAMEDOWNSCALEFACTOR', 'Escala de Frame');
define('MOTION_MINMOTIONAREA', 'Area Minima Movimento %:');


define('SCHED_HEADER','Agendamento de Gravacao');
define('ALRMSCHED_HEADER', 'Programação de alarmes');
define('SCHED_SELECTOR_TITLE', 'Use esses Seletores para Preencher o Cronograma Abaixo');
define('SCHED_OFF', 'Desligado');
define('SCHED_CONT', 'Continuo');
define('SCHED_MOTION', 'Movimento');
define('SCHED_SENSOR', 'Alarme');
define('SCHED_TRIGGER', 'Evento');
define('SCHED_FILLALL', 'Cheio');


define('ALL_DEVICES', 'Todos os Dispositivos');
define('EDITING_MMAP', 'Configuracao de Deteccao de Movimento para a Camera:');
define('EDITING_SCHED', 'Agendamento de Gravacao Para a Camera:');
define('EDITING_GLOBAL_SCHED', 'Agendamento Global');
define('GLOBAL_SCHED_OG', 'Usar Programacao Especifica Para Esta Camera.');
define('SIGNAL_TYPE_NOT_CONFD', 'Configurar Pelo Menos um Dispositivo Primeiro');
define('SCHED_GLOBAL', 'Global');
define('SCHED_SPEC', 'Dispositivo Especifico');
define('SCHED_SEL_TYPE', 'Selecione o Tipo de Gravacao e Clique no Dia / Hora Desejado Para Alterar o Tipo de Gravacao Para esse Periodo.');

define('HIDE_IMG', 'Mostrar / Ocultar Imagem');

define('AVAILABLE_DEVICES', 'Fontes de Video Disponiveis:');
define('LAYOUT_CONTROL', 'Selecionar Layout:');
define('NONE_SELECTED', 'Nenhum Selecionado');
define('CLEAR_LAYOUT', 'Limpar Layout');
define('BACK_TO_ADMIN', 'Retornar Menu ADM');

define('NO_NEWS', 'Nao ha Noticias no Momento.');

define('NO_CARDS', 'Nao ha Placas de Captura Detectadas Neste Sistema.');

define('NO_DEVICE_NAME', 'Campo Nome Nao Pode Estar em Branco.');
define('EXIST_NAME', 'Este Nome Ja Existe.');

/*user page errors*/
define('NO_USERNAME', 'Campo Login Nao Pode Estar em Branco.');
define('NO_EMAIL', 'Campo Email nao Pode Estar em Branco.');
define('NO_PASS', 'Campo Senha nao Pode Estar em Branco.');
define('CANT_REMOVE_ADMIN', 'Voce nao Pode Remover seus Proprios Privilegios de Administrador.');
define('USER_CREATED', 'Novo Usuario Criado.');
define('USER_DELETED', 'Registro do Usuario Excluido.');


#stats
define('STATS_HEARDER', 'Estatisticas do DVR:');
define('STATS_CPU', 'Utilizacao da CPU :');
define('STATS_MEM', 'Utilizacao da Memoria RAM:');
define('STATS_UPT', 'DVR Ligado a:');

#messages general
define('USER_KICKED', 'Sessao Finalizada Pelo Adminsitrador.');
define('USER_NACCESS', 'Voce Precisa de Privilegios de Administrador para Acessar Esta Pagina. Por Favor <a href="/">Faca Login Novamente</a>.');
define('USER_RELOGIN', 'Sessao Expirada, Por Favor <a htef="/">Faca Login Novamente</a>. ');
define('NO_CONNECTION', 'Nao Pode Conectar ao Servidor.<br /><br />Certifique-se de que o:<br />-Servidor HTTPD Esta em Execucao<br />-Voce Esta On-Line');

#active users 
define('ACTIVE_USERS_HEADER', 'Usuarios Conectados');
define('ACTIVE_USERS_SUBHEADER', 'Usuarios Atualmente Usando este Servidor DVR');
define('AU_IP', 'Endereco IP');
define('AU_KICK', 'Sessao Finalizada (Sistema Inoperante por 5 minutos)');
define('AU_BAN', 'Usuário Inopoerante');
define('AU_CANT_EOS', 'Voce Nao Pode Encerrar sua Sessao.');
define('AU_KICKED', 'Sessao Finalizada');
define('AU_CANT_SB', 'Voce nao Pode Encerrar sua Sessao');
define('AU_BANNED', 'Usuario Bloqueado.');
define('AU_INFO', 'Sessao Terminada Usuario Bloqueado por 5 minutos, Necessario Efetuar Login Novamente. Acessos e Configuracoes de Usuario Removida(Pode ser Restaurada na Pagina de Usuario).');
define('RCLIENT', 'Cliente Remoto');
define('WEB', 'WEB');
define('AC_ACCESS', '');
define('CLICK_HERE_TO_UPDATE', '[SIM]');

define('AIP_HEADER', 'Adicionar Camera IP');
define('DISCOVER_IP_CAMERAS', 'Localizar cameras IP');
define('AIP_NEW_LIST_AVAILABLE', 'Uma Versao Mais Recente da Nossa Lista de Cameras Suportadas esta Disponivel. Gostaria de Atualizar Para esta Versao?');
define('AIP_LIST_UPDATED_SUCCESSFULLY', 'Atualizacoes da Lista de Cameras.');
define('AIP_NEW_LIST_AVAIL', 'Novas Cameras IP estao Disponiveis.');
define('AIP_NEW_LIST_MYSQL_E', 'Nao Foi Possivel Atualizar o Banco de Dados, Verifique os Logs do Apache para Obter Detalhes');
define('AIP_CAN_NOT_GET_NEW_LIST', 'Nao foi Possivel Carregar as Novas Tabelas a Partir do Servidor. Por Favor Tente Mais Tarde');
define('AIP_SUBHEADER', '');
define('AIP_CAMERA_NAME', 'Nome');
define('AIP_CHOOSE_MANUF', 'Por Favor Selecione o Fabricante');
define('AIP_CHOOSE_MODEL', 'Por Favor Selecione o Modelo');
define('AIP_CHOOSE_FPSRES', 'Por Favor Selecione FPS/Resolucao');
define('AIP_CH_MAN_FIRST', 'Selecione Fabricante Primeiro');
define('AIP_CH_MOD_FIRST', 'Selecione Modelo Primeiro');
define('AIP_IP_ADDR_MJPEG', 'Endereco IP MJPEG');
define('AIP_PORT_MJPEG', 'Porta MJPEG');
define('AIP_IP_ADDR', 'Camera IP ou Nome');
define('AIP_CHOOSE_RES', 'Por Favor Selecione Framerate/Resolucao');
define('AIP_PORT', 'Porta RTSP');
define('AIP_USER', 'Nome de Usuario');
define('AIP_PASS', 'Senha');
define('AIP_MJPATH', 'Caminho do MJPEG');
define('AIP_RTSP', 'Caminho do RTSP');
define('AIP_ADD', 'Adicionar Camera');
define('AIP_ALREADY_EXISTS', 'A Camera Com Essas Configuracoes ja Existe na Lista de Dispositivos.');
define('AIP_NAME_ALREADY_EXISTS', 'A Camera com esse Nome ja Existe. Os Nomes Devem ser Exclusivos.');
define('AIP_NEEDIP', 'Insira o Endereco IP da Camera');
define('AIP_NEEDPORT', 'Insira o Numero da Porta RTSP');
define('AIP_NEEDMJPEGPORT', 'Insira o Numero da Porta MJPEG');
define('AIP_NEEDMJPEG', 'Insira Caminho para MJPEG');
define('AIP_NEEDRTSP', 'Por Favor Digite o caminho do RTSP');
define('AIP_ADVANCED_SETTINGS', 'Configuracoes Avancadas');
define('AIP_CAMADDED', 'A Camera IP foi Adicionada com Exito');
define('AIP_PROTOCOL', 'Fonte do Stream de Video');
define('AIP_ONVIF_PORT', 'Porta ONVIF ');
define('AIP_ONVIF_RESOLUTION', 'Resolução ONVIF');
define('AIP_CHECK_ONVIF_PORT', 'Checar');
define('AIP_CHECK_ONVIF_SUCCESS', 'Bem Sucedido');
define('AIP_CHECK_ONVIF_ERROR', 'Mal Sucedido');

#ip cam lib
define('COUND_NOT_CONNECT', 'Nao foi Possivel Conectar a Camera para Executar a Configuracao, Por Favor, Faca isso Manualmente');

define('IPCAM_MODEL', 'Modelo da Camera');
define('IPCAM_DEBUG_LEVEL', 'Ativar a Depuracao da Camera:');

define('DEVICE_EDIT_DETAILS', 'Propriedades');
define('DELETE_CAM', 'Deletar');
define('PROPS_HEADER', 'Propriedades da Camera IP');
define('IPCAM_WIKI_LINK', 'Sua camera IP Nao esta na Lista? Mantivemos uma Lista Maior de Caminhos MJPEG e RTSP <a href="http://www.rclindustria.com.br/2012/01/technical-information-list-of-mjpeg-and-rtsp-paths-for-network-cameras/">Aqui</a>.');
define('SERVER_NOT_RUNNING', 'Servidor Parado');
define('SERVER_RUNNING', 'Servidor Rodando');
define('NOT_UP_TO_DATE', 'Nova versao Disponivel');
define('UP_TO_DATE', 'Versao');
define('NOT_UP_TO_DATE_LONG', 'Foi Lancada uma Nova Versao do Servidor. E Altamente Recomendavel Atualizar.');
define('NOT_UP_TO_DATE_LINK', 'Para Notas de Lancamento e Instrucoes de Atualizacao, Visite Nosso <a href="http://forums.rclindustria.com.br/index.php?/forum/27-announcements/">forum</a>.');
define('WANT_TO_LEARN_MORE', 'Saber Mais...');
define('INSTALLED', 'Instalado');
define('CURR', 'Atual');
define('SETTINGS', 'Configuracoes');
define('DELTE_CAM_CONF', 'Voce tem Certeza Que Quer Deletar Esta Camera (ID:');

#layouts
define('NO_SAVED_LAYOUTS', 'Nao ha Layouts Salvos');
define('CHOOSE_LAYOUT', 'Layout Para Carregar...');
define('LAYOUTS', 'Layouts');

#login
define('PLEASE_LOGIN', 'Voce Precisa');

define('USERNAME_EXISTS', 'Um Usuario com Este Nome ja Existe.');

#PTZ
define('DEVICE_EDIT_PTZ', 'PTZ Configuracoes');

define('PTZ_SET_HEADER', 'Configuracoes PTZ para a Camera: ');
define('PTZ_PROTOCOL', 'Protocolo:');
define('PTZ_SET_SUBHEADER', 'Configuracao Basica PTZ ');
define('PTZ_CONTOL_PATH', 'Caminho do Controle:');
define('PTZ_ADDR', 'Endereco da Camera:');
define('PTZ_BAUD', 'Baud rate:');
define('PTZ_BIT', 'Bit:');
define('PTZ_STOP_BIT', 'Stop bit:');
define('PTZ_PARITY', 'Paridade:');

###
define('QUERY_FAILED', 'Nao foi Possivel Executar Consulta. Por Favor, Verifique <i>/var/log/apache2/error.log</i> Para Mais Detalhes.');
define('LOGOUT', 'Sair');
define('WRITE_FAILED', 'Falha ao Gravar na Pasta de Armazenamento em: ');
#mjpeg
define('MJPEG_DISABLED', 'Este Dispositivo foi Desativado.');
define('MJPEG_DEVICE_NOT_FOUND', 'Dispositivo com ID %ID% Nao foi Encontrado.');
define('MJPEG_NO_PERMISSION', 'Voce Nao tem Permissao para Acessar a Camera ID %ID%.');
#storage 
define('STORAGE_ESTIMATION', 'Estimativa de Armazenamento');
define('STRAGE_HEADER', 'Gerenciar Locais de Armazenamento');
define('ADD_LOCATION', 'Adicionar Localizacao');
define('LOCATION', 'Pasta:');
define('STORAGE_INFO_MESSAGE', 'Tenha Atencao ao Adicionar um novo Local de Armazenamento, A: <br /> - Pasta Existe <br /> - A Pasta esta Vazia <br /> - Pasta Pertence ao Usuario rcl, group rcl.');
define('DIR_DOES_NOT_EXIST_OR_NOT_READABLE', 'O servidor Nao Conseguiu Abrir o Diretorio Especificado "<b>%PATH%</b>", Verifique se ele Existe e se o Servidor Pode se Conectar a ele Executando esses Comandos no Terminal: <br /><br />
		Criar o Diretorio: "sudo mkdir -p %PATH%" <br />
		Definir Permissao: "sudo chmod 770 %PATH%; sudo chown rcl:rcl -R %PATH%".
	');
define('DIR_NOT_READABLE', 'Diretorio Especificado Existe, mas Nao e Legivel, Verifique se o Servidor tem Permissoes para Acessa-lo Executando Esses Comandos no Terminal:<br /><br />
		Definir Permissao: "sudo chmod 770 %PATH%; sudo chown rcl:rcl -R %PATH%".
	');
define('DIR_NOT_EMPTY', 'Diretorio Especificado Nao Esta Vazio, Todo o Conteudo Sera Excluido Apos Ele ser Adicionado.');
define('DIR_OK', 'Diretorio Especificado Existe e e Gravavel. Clique em "Salvar" para Adicionar esta Localizacao.');


define('ON_CARD', ' Placa de Captura ON ');
define('MAP_PRESET', 'Novo Preset...');

#logs
define('LOG_APACHE', 'Apache WEB Server Log');
define('LOG_PHP', 'PHP Error Log');
define('LOG_BC', 'RCL Log');

#ipcam statuses
define('IP_ACCESS_STATUS_F', 'Nao foi Possivel Conectar-se ao <b>%TYPE%</b>. Verifique se a Camera esta Ligada e se o Endereco IP esta Correto.');
define('IP_ACCESS_STATUS_404', 'Caminho <b>%TYPE%</b> Incorreto, Por Favor Verifique o Manual da Camera para Correta Configuracao.');
define('IP_ACCESS_STATUS_401', 'Login ou Senha Incorreta.');
define('IP_ACCESS_STATUS_303', 'Nao foi Possivel Resolver o  Hostname.');
define('IP_ACCESS_NCONFIG', 'Nao foi Possivel Conectar a Camera. Verificar Hostname/Endereco IP.');

#ip ptz presets
define('IPP_NEW', 'Criar um Conjunto Personalizado de Comandos PTZ');
define('IPP_DELETE', 'Tem Certeza de que Deseja Excluir Preset "');
define('IPP_HEADER', 'Selecionar PTZ Preset Para');
define('IPP_EDIT', 'Editar PTZ Preset');
define('IPP_EDIT_HELP', 'Preencha os Comandos Predefinidos de Acordo com as Especificacoes do Fabricante. <br /><br />As Seguintes Variaveis Sao Aceitas: <br /> 
							<b>%USERNAME%</b> - Usuario Para Acessar a Camera <br />
							<b>%PASSWORD%</b> - Senha para Acessar a Camera <br />
							<b>%ID%</b> - ID do Comando, ID e.g. Posicao Preset');
define('IPP_PRESET_NAME', 'Nome Preset');
define('IPP_PRESET_NAME_EX', 'Nome Preset');


define('IPP_PRESET_RIGHT', 'Direita');
define('IPP_PRESET_LEFT', 'Esquerda');
define('IPP_PRESET_UP', 'Acima');
define('IPP_PRESET_DOWN', 'Abaixo');
define('IPP_PRESET_UPRIGHT', 'Acima Direita');
define('IPP_PRESET_UPLEFT', 'Acima Esquerda');
define('IPP_PRESET_DOWNRIGHT', 'Abaixo Direita');
define('IPP_PRESET_DOWNLEFT', 'Abaixo Esquerda');
define('IPP_PRESET_TIGHT', 'Comprimento');
define('IPP_PRESET_TIGHT_EX', 'Mais Zoom');
define('IPP_PRESET_WIDE', 'Largura');
define('IPP_PRESET_WIDE_EX', 'Menos Zoom');
define('IPP_PRESET_FOCUSIN', 'Mais Focu');
define('IPP_PRESET_FOCUSOUT', 'Menos Focu');
define('IPP_PRESET_PRESET_SAVE', 'Salvar Preset');
define('IPP_PRESET_PRESET_GO', 'Va para o Preset');
define('IPP_PRESET_STOP', 'Pare');
define('IPP_PRESET_STOP_EX', 'Parar Movimento');
define('IPP_PRESET_NEEDS_STOP', 'Parada Necessaria');
define('IPP_PRESET_NEEDS_STOP_EX', 'Se a Camera Precisa de um Comando de Parada');
define('IPP_PRESET_HTTP_AUTH', 'Utilizar Autenticação HTTP');
define('IPP_PRESET_HTTP_AUTH_EX', 'Use Autenticacao HTTP ou parametros GET');
define('IPP_PRESET_PROTOCOL', 'Protocolo/Interface');
define('IPP_PRESET_PORT', 'Controle de Porta');
define('IPP_PRESET_PORT_EX', 'Especifique a Porta, Caso a Porta utilizada Nao for Padrao');
define('IPP_DISABLE_PTZ', 'Desabilitar PTZ para esta Camera');

#statistics
define('STS_SUBHEADER', 'Distribuicao de Eventos por Tipo e Intervalo de Datas');
define('STS_HEADER', 'Estatisticas do Evento');
define('STS_START_DATE', 'Data Inicial');
define('STS_END_DATE', 'Data Final');
define('STS_PR_GRP', 'Agrupamento Primario');
define('STS_SC_GRP', 'Agrupamento Secundario');
define('STS_ET_ALL', 'Todos Eventos');
define('STS_ET_M', 'Movimento');
define('STS_ET_C', 'Continuo');
define('STS_SUBMIT', 'Obter Estatisticas');
define('STS_TOTAL_EVENTS', 'Total <b>%TYPE%</b> de Eventos pelo Periodo:');
define('STS_PICKER_HEADER', 'Escolher o Intervalo de Datas e os Tipos de Eventos');
define('STS_PERCENTAGE_OF_TOTAL', 'Percentual do Total');
define('STS_NUM_EVENTS', 'Numero de Eventos');

#notifications
define('NTF_HEADER', 'Notificacoes de Email');
define('NTF_EXISTING', 'Regras Atuais');
define('NTF_SUBHEADER', 'Configurar Regras de Notificacoes de Email');
define('NTF_ADD_RULE', 'Adicionar uma Regra');
define('NTF_EDIT_RULE', 'Salvar Alteracoes');
define('NTF_ADD_RULE_TITLE', 'Adicionar Novas Regras para as Notificacoes');
define('NTF_ADD_RULE_DATETIME', 'Selecione Dia e Hora para as Notificacoes');
define('NTF_DAY', 'Dias da Semana');
define('NTF_START_TIME', 'Hora Inicial');
define('NTF_END_TIME', 'Hora Final');
define('NTF_CAMERAS', 'Escolher Cameras');
define('NTF_USERS', 'Usuario para Notificar');
define('NTF_LIMIT', 'Limite Maximo de Notificacoes');
define('NTF_LIMIT_LABEL', 'Limite de E-mails Enviados por Hora [0 para Ilimitado ]');
define('NTF_NO_RULES', 'Atualmente, Nao ha Regras de Notificacao');
define('NTF_EXISTING_RULES', 'Regras de Notificacao Atualmente em Vigor');

define('NTF_E_USERS', 'Necessario Selecionar Pelo Menos um Usuario');
define('NTF_E_DAYS', 'Necessario Selecionar Pelo Menos um Dia da Semana');
define('NTF_E_TIME', 'Necessario Hora de Inicio e Deve ser Anterior a Hora de Termino');
define('NTF_E_CAMS', 'Necessario Selecionar Pelo Menos uma Camera');

define('NTF_TEMPLATE', 'Enviar E-mail para <b>%USERS%</b> <br />on <b>%DAYS%</b> <br />between <b>%STIME%</b> and <b>%ETIME%</b> <br />Para Eventos on <b>%CAMERAS%</b>.');
define('NTF_T_TO', 'Enviar Email Para');
define('NTF_T_DOW', 'Nos Dias');
define('NTF_T_TIME', 'Entre Horas');
define('NTD_T_CAMS', 'Parar Eventos Em');
define('NTD_T_LIM', 'E-mails Por Hora');


define('DW_M', 'Segunda');
define('DW_T', 'Terca');
define('DW_W', 'Quarta');
define('DW_R', 'Quinta');
define('DW_F', 'Sexta');
define('DW_S', 'Sabado');
define('DW_U', 'Domingo');

#date/time related strings
define('DT_TODAY', 'Hoje');
define('DT_YESTERDAY', 'Ontem');
define('DT_THISWEEK', 'Esta Semana');
define('DT_LASTWEEK', 'Ultima Semana');
define('DT_QUICKDATE', 'Busca Rapida');
define('DT_DAYS', 'dias');
define('DT_HOURS', 'horas');
define('DT_MINUTES', 'minutos');

#backup/restore
define('BACKUP_HEADER', 'Faca Backup e Restaure seu Banco de Dados');
define('BACKUP_R_CONF', 'Restaurar o Banco de Dados ira Remover Permanentemente Todos os Dados Atuais.
Recomendamos Executar Backup da Base Atual Antes de Restaurar.');
define('BACKUP_SUBHEADER', 'Salvar um Backup do Banco de Dados ou Restaurar a Partir de um Backup Salvo');
define('BACKUP_B_TITLE', 'Backup do Banco de Dados');
define('BACKUP_PWD', 'Digite a Senha para o Usuario do Banco de Dados <b>%USERS%</b> Para o <b>%DBNAME%</b> Banco de Dados:');
define('BACKUP_B_OPT', 'Opcoes Adicionais');
define('BACKUP_B_OPT_NE', 'Nao Faca Backup de Dados de Eventos');
define('BACKUP_B_OPT_ND', 'Nao Faca Backup das Configuracoes de Dispositivo(s)');
define('BACKUP_B_OPT_NU', 'Nao Fazer Backup dos Registros do Usuario');
define('BACKUP_R_TITLE', 'Restaurar o Banco de Dados');
define('BACKUP_R_WRONG_FILETYPE', 'Espera-se que o Arquivo Seja um Arquivo Comprimido tar. Faca o Upload de um Arquivo de Backup Adequado.');
define('BACKUP_R_NOINFO', 'Nao Foi Possivel Ler o Arquivo de Informacoes de Backup. Isso Pode Significar que o Arquivo Esta Corrompido ou Contem Dados Invalidos. Processar Pode Causar Perda de Dados.');
define('BACKUP_R_LABEL', 'Escolha um Backup Salvo Anteriormente:');
define('BACKUP_R_INFO', 'Arquivos de Backup Enviados. <br /> <b>Data Backup:</b> %DATE% <br /> <b>Data Evento:</b> %E% <br /><b>Data Dispositivo:</b> %D%<br /><b>Data Usuario:</b> %U%');
define('BACKUP_FAILED', 'Nao Foi Possivel Executar o Backup. A Senha do Banco de Dados esta Correta?');
define('BACKUP_R_FAILED', 'Nao Foi Possivel Restaurar o Banco de Dados. Mysql Retornou o Seguinte Erro:<br />');
define('BACKUP_R_SUCCESS', 'O Banco de Dados Foi Restaurado com Exito. Atualize a Pagina.');
define('U_BACKUP', 'Backup');
define('U_UPLOAD', 'Upload');
define('U_RESTORE', 'Restaurar');
define('U_CANCEL', 'Cancelar');
define('U_INCLUDED', 'Incluida');
define('U_NOTINCLUDED', 'Nao Incluso');
define('BACKUP_R_WARN', 'Observe que Todas as Tabelas no Backup Serao Irreversivelmente Substituidas Quando a Restauracao Estiver Concluida. Digite a Senha do Banco de Dados para Continuar.');
define('BACKUP_DOWNLOAD', 'Download Arquivo Backup');
define('BACKUP_B_SUCCESS', 'Criada');

#licensing
define('L_ADDCODE', 'Adicionar Codigo de Licenca');
define('L_ADD', 'Adicionar');
define('L_PORTS', 'Portas');
define('L_CONFIRMCODE', 'Confirmar Codigo de Licenca');
define('L_CONFIRMCODE_MSG', 'Infelizmente, Nao Foi Possivel Confirmar a Licenca Automaticamente. Verifique a sua Internet e Tente Novamente, ou <a href="mailto:suporte@rclindustria.com.br"> Entre em Contato</a> Para Obter Codigo de Autorizacao Usando Este ID Exclusivo: ');
define('L_INVALID_LICENSE', 'O Codigo de Licenca que Introduziu Nao Parece Ser Valido. Certifique-se de Que Foi Digitado ou Copiado Exatamente Como Fornecido. Se Continuar a Ter Este Problema, Por Favor <a href="mailto:suporte@rclindustria.com.br">Entre em Contato</a> Com a RCL Industria.');
define('L_INVALID_LICENSE_EXISTS', 'O Codigo de Licenca que Introduziu ja Esta a Ser Utilizado Neste Sistema.');
define('L_INVALID_CONFIRMATION', 'O Codigo de Confirmacao que Introduziu e Invalido. Certifique-se de que foi Digitado ou Copiado Exatamente Como Fornecido. Se Continuar a Ter Este Problema, Por Favor <a href="mailto:suporte@rclindustria.com.br"> Entre em Contato </a> Com a RCL Industria.');
define('L_LICENSE_ADDED', 'Licenca Ativada com Sucesso.');
define('L_CURRENT',  'Codigos de Licenca Ativa');
define('L_CONFIRM', 'Confirmar');
define('L_CONFIRM_DELETE', 'Tem Certeza de Que Deseja Remover esta Licenca?');
define('L_NO_LICENSES', 'Voce Nao Tem Licencas Ativas');
define('L_AUTO_RESP_1', 'Este Codigo de Licenca nao Existe.');
define('L_AUTO_RESP_2', 'Esta Licenca ja Foi Usada em uma Maquina Diferente. Se Voce Recentemente Fez Alteracoes na Configuracao do Computador, <a href="mailto:suporte@rclindustria.com.br">Contate a RCL</a> Para Reiniciar a Sua Licenca.');
define('L_AUTO_RESP_3', 'Desculpe, Voce Fez Muitas Tentativas de Autorizacao Sem Exito Por Dia. Por Favor <a href="mailto:suporte@rclindustria.com.br">Contate a RCL</a> ou Tente Novamente Mais Tarde.');
define('L_AUTO_RESP_4', 'Necessario Contatar a RCL Atraves do Telefone ou do E-mail Para Obter a Autorizacao.');
define('L_AUTO_RESP_5', 'Ocorreu um Erro ao Autorizar a Sua Licenca. Tente Novamente Mais Tarde ou <a href="mailto:suporte@rclindustria.com.br">Contate a RCL</a>.');
#Cam specific
define('ACTI_STREAMING_SET_3', 'Sua Camera ACTi Foi Automaticamente Configurada para RTP por Streaming UDP.');
define('ACTI_STREAMING_N_SET_3', 'Sua Camera ACTi Nao Pode Ser Configurada Automaticamente para RTP por Streaming UDP. Faca isso Manualmente.');

define('VIVOTEC_SETRAMING_SET', 'Sua Camera Vivotek Foi Configurada Automaticamente Para Usar o Codec MJPEG no Segundo Codificador.');
define('VIVOTEC_SETRAMING_N_SET', 'A Camera Vivotek Nao Pode Ser Configurada Automaticamente Para Usar o Codec MJPEG no Segundo Codificador. Faca isso Manualmente.');

define('L_DELETE', 'Apagar');
define('L_EDIT', 'Editar');
define('L_NONE', 'Nenhum');
define('U_LOADING', 'Carregando...');
define('L_ENABLE', 'Habilitar');
define('L_DISABLE', 'Desativar');

define('BETA_LICENSE_WARNING', 'Observe que Voce Esta Executando Uma Versao Demo do Software RCL DVR e Estara Ativo Por 30 dias. Por Favor Visite <a href="http://loja.rclindustria.com.br/rcl-dvr/software-licenca">Loja online</a> Para Adquirir Sua Licenca. <br /><br /> Obrigado Por Seu Apoio Continuo,<br /> RCL Industria.');

#ip cam props
define('IPCAM_TCPUDP_LEVEL', 'Transporte');

#playback
define('PLAYBACK_FILE_NOT_FOUND', 'MKV Arquivo Nao Foi Encontrado.');


#auditlog
define('AUDITLOG_HEADER', 'Log de auditoria');
define('AUDITLOG_DATETIME', 'Data');
define('AUDITLOG_USER', 'Usuário');
define('AUDITLOG_EVENTTYPE', 'Eventos');
define('AUDITLOG_DEVICE', 'Dispositivo');
define('AUDITLOG_IP', 'IP');

#changepassword
define('CHANGEPWD_PLEASE', 'Por favor Insira Nova Senha');
define('CHANGEPWD_NEWPWD', 'Nova Senha');
define('CHANGEPWD_CONFIRM', 'Confirmar Senha');
define('CHANGEPWD_CHANGE', 'Alterar Senha');

#login
define('LOGIN_PLEASE', 'Faça o Login');
define('LOGIN_PASSWORD', 'Senha');
define('LOGIN_LOGIN', 'Usuario');

?>
