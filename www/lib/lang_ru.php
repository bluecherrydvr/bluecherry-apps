<?php defined('INDVR') or exit();

/*
 * Copyright 2010-2019 Bluecherry, LLC
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#company global
define('DVR_COMPANY_NAME', 'Bluecherry');
define('DVR_DVR', 'DVR');

#headers
define('PAGE_HEADER_MAIN', 'Администрирование');
define('PAGE_HEADER_VIEWER', 'Просмотр в реальном времени');
define('PAGE_HEADER_SCHED', 'Доступ в настоящее время запрещён');

define('PAGE_HEADER_LOGIN', 'Вход');
define('PAGE_HEADER_SEPARATOR', ' :: ');

#main menu
define('MMENU_NEWS', 'Объявления');
define('MMENU_GS', 'Общие настройки');
define('MMENU_STORAGE', 'Хранилище');
define('MMENU_USERS', 'Пользователи');
define('MMENU_LOG', 'Системный журнал');
define('MMENU_DEVICES', 'Устройства');
define('MMENU_SCHED', 'Глобальное расписание');
define('MMENU_LIVEVIEW', 'Просмотр в реальном времени');
define('MMENU_PLAYBACK', 'Воспроизведение');
define('MMENU_STATISTICS', 'Статистика событий');
define('MMENU_MONITORING', 'Мониторинг системы');
define('MMENU_ACTIVE_USERS', 'Подключённые пользователи');
define('MMENU_CLIENT_DOWNLOAD', 'Скачать удалённый клиент');
define('MMENU_DOCUMENTATION', 'Документация');
define('MMENU_NOTFICATIONS', 'Email‑уведомления');
define('MMENU_BACKUP', 'Резервная копия БД');


#main
define('COUND_NOT_OPEN_PAGE', 'Страницу не удалось загрузить');
define('COUND_NOT_OPEN_PAGE_EX', 'Запрошенная страница сейчас недоступна. Убедитесь, что можете подключиться к серверу DVR и что сервер запущен.');

#dsched
define('DSCED_HEADER', 'Доступ к онлайн‑просмотру для этой учётной записи сейчас запрещён.');
define('DSCED_MSG', 'Вы не можете использовать веб‑интерфейс в данный момент. Если это ошибка — свяжитесь с администратором.');


#die (fatal) messages
define('LANG_DIE_COULDNOTCONNECT', 'Критическая ошибка: не удалось подключиться к базе данных: ');
define('LANG_DIE_COULDNOTOPENCONF', 'Критическая ошибка: не удалось прочитать файл конфигурации.');

#general
define('BB_MAP_ARROW', '>');
define('SAVE_SETTINGS', 'Сохранить настройки');
define('SAVE_CHANGES', 'Сохранить изменения');
define('SAVE', 'Сохранить');
define('SAVE_AS', 'Сохранить как…');
define('CLEAR_ALL', 'Очистить всё');

define('CHANGES_OK', 'Изменения успешно сохранены');
define('CHANGES_FAIL', 'Не удалось применить изменения. Повторите позже.');

define('ENABLE_DEVICE_NOTENOUGHCAP', '<b>У карты недостаточно ресурсов для этого устройства. <br/></b>Уменьшите разрешение/кадровую частоту других устройств на этой карте.');
define('NEW_DEV_NEFPS', 'Устройство настроено по умолчанию, но ресурсов карты недостаточно для его включения. Уменьшите разрешение/кадровую частоту других устройств и попробуйте снова.');

#general settings
define('GLOBAL_SETTINGS', 'Общие настройки DVR');
define('G_DISABLE_VERSION_CHECK', 'Проверять доступные обновления');
define('G_DISABLE_VERSION_CHECK_EX', 'требуется доступ сервера в Интернет');
define('G_DVR_NAME', 'Имя DVR');
define('G_DVR_NAME_EX', 'Можно назвать DVR по местоположению или назначению');
define('G_DVR_EMAIL', 'Email‑адрес');
define('G_DVR_EMAIL_EX', 'Email для уведомлений');
define('G_DEV_NOTES', 'Заметки');
define('G_DEV_NOTES_EX', 'памятки DVR');
define('G_DISABLE_IP_C_CHECK', 'Отключить проверку доступности IP‑камер');
define('G_DISABLE_IP_C_CHECK_EX', 'Может повысить производительность веб‑интерфейса');
define('G_DISABLE_WEB_STATS', 'Скрыть статистику в веб‑интерфейсе');
define('G_DISABLE_WEB_STATS_EX', 'Может повысить производительность веб‑интерфейса');
define('G_DATA_SOURCE', 'Источник списка');
define('G_DATA_SOURCE_LOCAL', 'Локальный список');
define('G_DATA_SOURCE_LIVE', 'Онлайн (нужно активное подключение к Интернету)');

define('G_SMTP_TITLE', 'Настройка SMTP‑сервера');
define('G_SMTP_TITLE_TEST', 'Проверка конфигурации SMTP‑сервера');
define('G_PERF_TITLE', 'Производительность/подключение');

define('G_MAX_RECORD_TIME', 'Длина записи');
define('G_MAX_RECORD_TIME_EX', 'Делить записи на сегменты по');

define('G_MAX_RECORD_AGE', 'Макс. срок хранения записей');
define('G_MAX_RECORD_AGE_EX', 'Автоматически удалять записи старше указанного количества дней (опционально)');

define('G_VAAPI_DEVICE', 'Устройство VAAPI');
define('G_VAAPI_DEVICE_EX', 'Render‑узел, используемый для аппаратного ускорения VAAPI при декодировании/кодировании видео');

define('G_SUBDOMAIN_TITLE', 'Настройка SSL / CNAME');
define('G_SUBDOMAIN_API_BASE_URL_NAME', 'Базовый URL поддомена');
define('G_SUBDOMAIN_API_BASE_URL_NAME_EX', 'Можно указать базовый URL провайдера поддомена');

define('G_LIVEVIEW_VIDEO_TITLE', 'Настройка просмотра в реальном времени');
define('G_LIVEVIEW_VIDEO_METHOD', 'Метод видео');

#mail
define('G_SMTP_SERVICE', 'Почтовый сервис');
define('G_SMTP_EMAIL_FROM', 'Значение «From» SMTP');
define('G_SMTP_CREDENTIALS', 'Учетные данные');
define('G_SMTP_USERNAME', 'Имя пользователя');
define('G_SMTP_PASSWORD', 'Пароль');
define('G_SMTP_HOST', 'Хост SMTP');
define('G_SMTP_PORT', 'Порт SMTP');
define('G_SMTP_SSL', 'SSL');
define('G_SMTP_TRANSPORT_SECURITY', 'Транспортная безопасность');
define('G_SMTP_SERVICE_EX', '');
define('G_SMTP_EMAIL_FROM_EX', 'Когда Bluecherry отправляет email, это значение используется как адрес отправителя («From»)');
define('G_SMTP_USERNAME_EX', '');
define('G_SMTP_PASSWORD_EX', '');
define('G_SMTP_HOST_EX', 'например, smtp.googlemail.com');
define('G_SMTP_PORT_EX', '');
define('G_SMTP_SSL_EX', '');
define('G_SMTP_FAILED', 'Не удалось отправить письмо с данной конфигурацией.');

define('G_SMTP_DEF_MTA', 'Системный MTA по умолчанию');
define('G_SMTP_SMTP', 'Пользовательский SMTP');
define('G_SEND_TEST_EMAIL_TO', 'Отправить тестовое письмо на');
define('G_SEND_TEST_EMAIL', 'Отправить');
define('G_TEST_EMAIL_SUBJECT', 'Тестовое письмо');
define('G_TEST_EMAIL_BODY', 'Тестовое письмо');
define('G_TEST_EMAIL_SENT', 'Сообщение отправлено');


#users page
define('EDIT_ACCESS_LIST', 'Права доступа к камерам');
define('USERS_ALL_TABLE_HEADER', 'Пользователи DVR');
define('USERS_DETAIL_TABLE_HEADER', 'Сведения о пользователе: ');

define('CAMERA_PERMISSIONS_SUB', 'Права доступа пользователя к камерам');
define('RESTRICT_ALL', 'Запретить всё');
define('ALLOW_ALL', 'Разрешить всё');
define('CAMERA_PERMISSIONS_LIST_TITLE', 'нажмите на имена камер, чтобы разрешить/запретить доступ');

define('USERS_ALL_TABLE_SUBHEADER', 'Пользователи, зарегистрированные в системе DVR.');
define('USERS_DETAIL_TABLE_SUBHEADER', 'Редактировать информацию и права доступа пользователя. ');

define('USERS_TABLE_HEADER_NAME', 'Полное имя');
define('USERS_TABLE_HEADER_LOGIN', 'Логин');
define('USERS_TABLE_HEADER_EMAIL', 'Email');
define('USERS_TABLE_HEADER_STATUS', 'Статус');
define('USERS_LIST', 'Пользователи DVR');

define('DELETE_USER', 'Удалить пользователя');
define('DELETE_USER_SELF', 'Вы не можете удалить свою запись пользователя');
define('USER_DELETED_OK', 'Запись пользователя успешно удалена');

define('USERS_NEW', 'Создать пользователя');
define('USERS_NAME', 'Имя');
define('USERS_NAME_EX', 'полное имя пользователя');
define('USERS_LOGIN', 'Логин');
define('USERS_LOGIN_EX', 'логин');
define('USERS_PASSWORD', 'Пароль');
define('USERS_PASSWORD_EX', 'пароль');
define('USERS_PASSWORD_CURRENT', 'Текущий пароль');
define('USERS_PASSWORD_NEW', 'Новый пароль');
define('USERS_PASSWORD_WRONG', 'Неверный текущий пароль.');
define('USERS_PHONE', 'Телефон');
define('USERS_PHONE_EX', 'номер телефона');
define('USERS_ACCESS_SETUP', 'Доступ администратора');
define('USERS_ACCESS_SETUP_EX', 'административные привилегии');
define('USERS_ACCESS_REMOTE', 'Удалённый доступ');
define('USERS_ACCESS_REMOTE_EX', 'доступ удалённого клиента');
define('USERS_ACCESS_WEB', 'Веб‑доступ');
define('USERS_ACCESS_WEB_EX', 'доступ к веб‑просмотру');
define('USERS_ACCESS_BACKUP', 'Архивное видео');
define('USERS_ACCESS_BACKUP_EX', 'разрешить доступ к прошлым событиям');
define('USERS_EMAIL', 'Email‑адреса');
define('USERS_EMAIL_EX', 'для email‑уведомлений');
define('EMAIL_LIMIT', 'Лимит');
define('EMAIL_LIMIT_EX', 'Email/час (0 — без ограничений)');
define('USERS_NOTES', 'Заметки пользователя');
define('USERS_NOTES_EX', 'видны только администратору');
define('USERS_CHANGEPASSWORD', 'Сменить пароль при следующем входе');
define('USERS_CHANGEPASSWORD_EX', 'Пользователю будет предложено сменить пароль');

define('USERS_STATUS_ADMIN', 'Администратор');
define('USERS_STATUS_VIEWER','Просмотр');
define('USERS_STATUS_DISABLE', 'Отключён');


define('USERS_STATUS_SETUP', 'Админ');

#log
define('LOG_EMPTY', 'Текущий файл журнала пуст.');
define('LOG_ALL', 'Весь журнал');
define('LOG_COULD_NOT_OPEN', 'Не удалось открыть файл журнала');
define('LOG_HOW_MANY', 'Сколько последних строк загрузить…');
define('LOG_FILE_DOES_NOT_EXIST', 'Файл журнала не существует');
define('LOG_DOWNLOAD', 'Скачать отладочную информацию');

define('SCROLL_TO_BOTTOM', 'прокрутить вниз');
define('SCROLL_TO_TOP', 'прокрутить вверх');

#news
define('NEWS_HEADER', DVR_COMPANY_NAME.' — объявления');

#login
define('LOGIN_WRONG', 'Неверная пара логин/пароль. Попробуйте снова.');
define('LOGIN_OK', 'Пожалуйста, подождите, выполняется перенаправление…');
define('NA_WEB', 'У вас нет прав для использования веб‑интерфейса');
define('NA_CLIENT', 'У вас нет прав для использования удалённого клиента');

define('WARN_DEFAULT_PASSWORD', 'Вы не изменили пароль по умолчанию в системе. В целях безопасности настоятельно рекомендуем <a id="updateAdminPassword" href="#">обновить пароль</a>.');

#devices
define('CARD_HEADER', 'Аппаратное сжатие Bluecherry');
define('CARD_CHANGE_ENCODING', 'нажмите, чтобы изменить кодек');
define('LOCAL_DEVICES', 'Локальные устройства:');
define('PORT', 'Порт');
define('DEVICE_VIDEO_SOURCE', 'Источник видео');
define('DEVICE_VIDEO_STATUS', 'Статус устройства');
define('DEVICE_VIDEO_STATUS_OK', 'Включено');
define('DEVICE_VIDEO_STATUS_CHANGE_OK', 'Отключить');
define('DEVICE_VIDEO_STATUS_disabled', 'Отключено');
define('DEVICE_VIDEO_STATUS_CHANGE_disabled', 'Включить');
define('DEVICE_VIDEO_NAME_notconfigured', 'Не настроено');
define('DEVICE_VIDEO_STATUS_notconfigured', 'Отключено');
define('DEVICE_VIDEO_STATUS_CHANGE_notconfigured', 'Включить');
define('DEVICE_VIDEO_RESFPS', 'Разрешение/кадровая частота');
define('DEVICE_EDIT_MMAP', 'Настройки детекции движения');
define('DEVICE_EDIT_VIDADJ', 'Настройки видео/аудио');
define('DEVICE_EDIT_SCHED', 'Расписание');
define('DEVICE_EDIT_ONCAM', 'Настройка на камере'); 
define('VIDEO_ADJ', 'Настройки видео для');
define('DEVICE_UNNAMED', 'Без имени');
define('TOTAL_DEVICES', 'Всего устройств');
define('ENABLE_ALL_PORTS', 'включить все порты');

define('VA_HEADER', 'Настройки видео');
define('VA_BRIGHTNESS', 'Яркость');
define('VA_HUE', 'Оттенок');
define('VA_AUDIO', 'Усиление аудио');
define('VA_AUDIO_ENABLE', 'Включить аудио');
define('VA_SATURATION', 'Насыщенность');
define('VA_CONTRAST', 'Контраст');
define('VA_VIDEO_QUALITY', 'Качество видео');
define('VA_SETDEFAULT', 'Сбросить значения');

define('DEVICE_ENCODING_UPDATED', 'Настройка видеостандарта изменена. <hr /> <b>Чтобы изменения вступили в силу, перезапустите сервер.</b><br>');
define('DB_FAIL_TRY_LATER', 'Не удалось применить изменения. Убедитесь, что можете подключиться к серверу DVR и что сервер запущен.');

define('MMAP_HEADER', 'Настройки детекции движения');
define('MMAP_SELECTOR_TITLE', 'Используйте селекторы, чтобы выбрать чувствительность и заполнить сетку ниже');
define('BUFFER_TITLE', 'Сколько секунд записывать до и после события движения');
define('PRE_REC_BUFFER', 'Буфер до события');
define('POST_REC_BUFFER', 'Буфер после события');
define('MOTION_ALGORITHM_TITLE', 'Алгоритм детекции движения');
define('MOTION_ALGORITHM', 'Алгоритм');
define('MOTION_DEFAULT', 'По умолчанию');
define('MOTION_EXPERIMENTAL', 'Экспериментальный');
define('MOTION_CV_TEMPORAL', 'Покадровый');

define('SCHED_HEADER','Расписание записи устройства');
define('SCHED_SELECTOR_TITLE', 'Используйте селекторы, чтобы заполнить расписание ниже');
define('SCHED_OFF', 'Выкл.');
define('SCHED_CONT', 'Непрерывно');
define('SCHED_MOTION', 'Движение');
define('SCHED_CONTMOTION', 'Непрерывно+Движение');
define('SCHED_SENSOR', 'Датчик');
define('SCHED_TRIGGER', 'Триггер');
define('SCHED_FILLALL', 'Заполнить');


define('ALL_DEVICES', 'Все устройства');
define('EDITING_MMAP', 'Настройки детекции движения для камеры:');
define('EDITING_SCHED', 'Расписание записи для камеры:');
define('EDITING_GLOBAL_SCHED', 'Глобальное расписание');
define('GLOBAL_SCHED_OG', 'Использовать индивидуальное расписание для этой камеры.');
define('TRIGGER_ONVIF_EVENTS', 'Запускать запись по событиям ONVIF');
define('SIGNAL_TYPE_NOT_CONFD', 'Сначала настройте хотя бы одно устройство');
define('SCHED_GLOBAL', 'глобальное');
define('SCHED_SPEC', 'для устройства');
define('SCHED_SEL_TYPE', 'Выберите тип записи и нажмите на нужный день/час, чтобы изменить режим записи для периода.');

define('HIDE_IMG', 'Показать/скрыть изображение');

define('AVAILABLE_DEVICES', 'Доступные источники видео:');
define('LAYOUT_CONTROL', 'Выберите макет:');
define('NONE_SELECTED', 'Ничего не выбрано');
define('CLEAR_LAYOUT', 'Очистить макет');
define('BACK_TO_ADMIN', 'Вернуться в панель администратора');

define('NO_NEWS', 'Сейчас новостей нет.');

define('NO_CARDS', 'В системе не обнаружены поддерживаемые платы видеозахвата.');

define('NO_DEVICE_NAME', 'Поле имени не может быть пустым.');
define('EXIST_NAME', 'Такое имя уже существует.');

/*user page errors*/
define('NO_USERNAME', 'Поле логина не может быть пустым.');
define('NO_EMAIL', 'Поле email не может быть пустым.');
define('NO_PASS', 'Поле пароля не может быть пустым.');
define('CANT_REMOVE_ADMIN', 'Вы не можете снять собственные права администратора.');
define('USER_CREATED', 'Создана новая запись пользователя.');
define('USER_DELETED', 'Запись пользователя удалена.');


#stats
define('STATS_HEARDER', 'Server statistics:');
define('STATS_CPU', 'CPU usage:');
define('STATS_MEM', 'Memory usage:');
define('STATS_UPT', 'Server uptime:');

#messages general
define('USER_KICKED', 'Your session was terminated by an Administrator.');
define('USER_NACCESS', 'You need administrator priveleges to access this page. Please <a href="/">login again</a>.');
define('USER_RELOGIN', 'Your session has expired, please <a htef="/">relogin</a>. ');
define('NO_CONNECTION', 'Could not connect to the server.<br /><br />Please make sure that:<br />-server httpd is running<br />-you are online');

#active users 
define('ACTIVE_USERS_HEADER', 'Connected users');
define('ACTIVE_USERS_SUBHEADER', 'Users currently using this DVR server');
define('AU_IP', 'IP address');
define('AU_KICK', 'End session (bans user for 5 minutes)');
define('AU_BAN', 'Ban user');
define('AU_CANT_EOS', 'You can not terminate your own session.');
define('AU_KICKED', 'User session terminated');
define('AU_CANT_SB', 'You can not ban yourself');
define('AU_BANNED', 'User is banned.');
define('AU_INFO', 'Kicking user will end session from that IP address, ban user for 5 minutes and require re-login. Banning user will remove all access priveleges (can be restored on user page).');
define('RCLIENT', 'Remote Client');
define('WEB', 'Web');
define('AC_ACCESS', '');
define('CLICK_HERE_TO_UPDATE', '[Yes]');

define('AIP_HEADER', 'Add an IP Camera');
define('DISCOVER_IP_CAMERAS', 'Discover IP Cameras');
define('AIP_NEW_LIST_AVAILABLE', 'It appears that a newer version of our supported camera list is available. Would you like to upgrade to this version?');
define('AIP_LIST_UPDATED_SUCCESSFULLY', 'Camera list updates sussessfully.');
define('AIP_NEW_LIST_AVAIL', 'New ip camera tables are available.');
define('AIP_NEW_LIST_MYSQL_E', 'Could not update the database, please check Apache logs for details');
define('AIP_CAN_NOT_GET_NEW_LIST', 'Could not load the new tables from the server. Please try later');
define('AIP_SUBHEADER', '');
define('AIP_CAMERA_NAME', 'Camera name');
define('AIP_CHOOSE_MANUF', 'Please choose manufacturer');
define('AIP_CHOOSE_MODEL', 'Please choose model');
define('AIP_CHOOSE_FPSRES', 'Please choose fps/resolution');
define('AIP_CH_MAN_FIRST', 'Choose manufacturer first');
define('AIP_CH_MOD_FIRST', 'Choose model first');
define('AIP_IP_ADDR_MJPEG', 'MJPEG IP Address');
define('AIP_PORT_MJPEG', 'MJPEG Port');
define('AIP_IP_ADDR', 'Camera IP or Host');
define('AIP_CHOOSE_RES', 'Please choose framerate/resolution');
define('AIP_PORT', 'RTSP Port');
define('AIP_USER', 'Username');
define('AIP_PASS', 'Password');
define('AIP_MJPATH', 'Path to MJPEG');
define('AIP_RTSP', 'Path to RTSP');
define('AIP_SUBSTREAM_ENABLE', 'Использовать дополнительный поток для просмотра в реальном времени');
define('AIP_SUBSTREAM_PATH', 'Путь дополнительного потока');
define('AIP_ADD', 'Add camera');
define('AIP_ALREADY_EXISTS', 'Camera with these settings already exists in the list of the devices.');
define('AIP_NAME_ALREADY_EXISTS', 'Camera with this name already exists. Names must be unique.');
define('AIP_NEEDIP', 'Please enter ip address of the camera');
define('AIP_NEEDPORT', 'Please enter RTSP port number');
define('AIP_NEEDMJPEGPORT', 'Please enter MJPEG port number');
define('AIP_NEEDMJPEG', 'Please enter path to MJPEG');
define('AIP_NEEDRTSP', 'Please enter path to RTSP');
define('AIP_ADVANCED_SETTINGS', 'advanced settings');
define('AIP_CAMADDED', 'IP camera has been successfully added');
define('AIP_PROTOCOL', 'Video stream source');
define('AIP_ONVIF_PORT', 'ONVIF port');
define('AIP_CHECK_ONVIF_PORT', 'ONVIF Probe & Autoconfigure');
define('AIP_CHECK_ONVIF_SUCCESS', 'Successfull');
define('AIP_CHECK_ONVIF_ERROR', 'Unsuccessful');
define('AIP_LIMIT_ALLOWED_DEVICES', 'Could not add a camera, because exceeds the limit of the allowed devices.');
define('AIP_TEST_CONNECTION', 'Test Connection');
define('AIP_TEST_CONNECTION_MESSAGE', 'Test connection to IP Camera');
define('AIP_CONNECTION_SUCCESS', 'Connection Successful using %TYPE%');
define('AIP_CONNECTION_FAIL', 'Connection Unsuccessful using %TYPE%');

# HLS configuration
define('AIP_HLS_WINDOW_SIZE', 'HLS window size');
define('AIP_HLS_SEGMENT_SIZE', 'HLS segment size');
define('AIP_HLS_SEGMENT_SIZE_EX', 'You can specify HLS segment size in bytes (0 means unlimited)');
define('AIP_HLS_SEGMENT_DURATION', 'HLS segment duration');

#ip cam lib
define('COUND_NOT_CONNECT', 'Could not connect to the camera to perform the setup, please do so manually');

define('IPCAM_MODEL', 'Camera model');
define('IPCAM_DEBUG_LEVEL', 'Turn camera debugging on:');

define('DEVICE_EDIT_DETAILS', 'Properties');
define('DELETE_CAM', 'Delete');
define('PROPS_HEADER', 'IP Camera properties');
define('IPCAM_WIKI_LINK', 'Is your IP camera not in the list?  We maintain a larger list of MJPEG and RTSP paths <a href="https://www.bluecherrydvr.com/supported-ip-cameras/">here</a>.  If you run into problems, please <a href="https://www.bluecherrydvr.com/chat"> join us on Gitter or Matrix (https://www.bluecherrydvr.com/chat/)</a> and we\'ll try to add support for your camera.');
define('SERVER_NOT_RUNNING', 'Server process stopped.');
define('SERVER_RUNNING', 'Server process running');
define('NOT_UP_TO_DATE', 'New version available.');
define('UP_TO_DATE', 'Version');
define('NOT_UP_TO_DATE_LONG', 'New version of the server was released. We strongly recommend to update.');
define('NOT_UP_TO_DATE_LINK', 'For release notes and update instructions, please visit our <a href="http://forums.bluecherrydvr.com/index.php?/forum/27-announcements/">forums</a>.');
define('WANT_TO_LEARN_MORE', 'Learn more...');
define('INSTALLED', 'Installed');
define('CURR', 'Current');
define('SETTINGS', 'Settings');
define('DELTE_CAM_CONF', 'Are you sure you want to delete this camera (ID:');

#layouts
define('NO_SAVED_LAYOUTS', 'Сохранённых макетов нет');
define('CHOOSE_LAYOUT', 'Загрузить макет…');
define('LAYOUTS', 'Макеты');

#login
define('PLEASE_LOGIN', 'Вам необходимо');

define('USERNAME_EXISTS', 'Пользователь с таким логином уже существует.');

#reencode
define('REENCODE_HEADER', 'Настройка пере‑кодирования трансляции');
define('REENCODE_SUBHEADER', 'Параметры пере‑кодирования трансляции для камеры');
define('REENCODE_ENABLED', 'Включить пере‑кодирование трансляции');
define('REENCODE_BITRATE', 'Выходной битрейт');
define('REENCODE_RESOLUTION', 'Выходное разрешение');
define('DEVICE_EDIT_REENCODE', 'Настройки пере‑кодирования трансляции');

#PTZ
define('DEVICE_EDIT_PTZ', 'Настройки PTZ');

define('PTZ_SET_HEADER', 'Настройки PTZ для камеры: ');
define('PTZ_PROTOCOL', 'Протокол:');
define('PTZ_SET_SUBHEADER', 'Базовая конфигурация PTZ');
define('PTZ_CONTOL_PATH', 'Путь управления:');
define('PTZ_ADDR', 'Адрес камеры:');
define('PTZ_BAUD', 'Скорость (baud):');
define('PTZ_BIT', 'Бит:');
define('PTZ_STOP_BIT', 'Стоп‑бит:');
define('PTZ_PARITY', 'Чётность:');

###
define('QUERY_FAILED', 'Не удалось выполнить запрос. Подробности смотрите в <i>/var/log/apache2/error.log</i>.');
define('LOGOUT', 'выход');
define('WRITE_FAILED', 'Не удалось записать в каталог хранилища: ');

#mjpeg & hls
define('MJPEG_DISABLED', 'Это устройство отключено.');
define('MJPEG_DEVICE_NOT_FOUND', 'Устройство с ID %ID% не найдено.');
define('MJPEG_NO_PERMISSION', 'У вас нет прав доступа к камере ID %ID%.');
define('MJPEG_DEVICE_ID_NOT_SUPPLIED', 'ID устройства не указан.');

#storage 
define('STORAGE_HEADER', 'Управление местами хранения');
define('ADD_LOCATION', 'Добавить расположение');
define('LOCATION', 'Папка:');
define('STORAGE_INFO_MESSAGE', 'Обратите внимание: при добавлении нового места хранения необходимо убедиться, что: <br /> - папка существует <br /> - папка пуста <br /> - папка принадлежит пользователю bluecherry, группе bluecherry.');
define('DIR_DOES_NOT_EXIST_OR_NOT_READABLE', 'Server could not open the specified directory "<b>%PATH%</b>". See Note 2.
	');
define('DIR_NOT_WRITABLE', 'Указанный каталог "<b>%PATH%</b>" существует, но недоступен для записи. Смотрите Примечание 2.');
define('DIR_NOT_READABLE', 'Указанный каталог "<b>%PATH%</b>" существует, но недоступен для чтения. Смотрите Примечание 2.
	');
define('DIR_NOT_EMPTY', 'Указанный каталог не пуст. Всё содержимое будет удалено после добавления.');
define('DIR_OK', 'Указанный каталог существует и доступен для записи. Нажмите «Сохранить», чтобы добавить.');


define('ON_CARD', ' на карте ');
define('MAP_PRESET', 'Новый пресет…');

#logs
define('LOG_APACHE', 'Журнал веб‑сервера Apache');
define('LOG_PHP', 'Журнал ошибок PHP');
define('LOG_BC', 'Журнал Bluecherry');

#ipcam statuses
define('IP_ACCESS_STATUS_F', 'Не удалось подключиться к <b>%TYPE%</b>. Проверьте, что камера включена и IP‑адрес указан верно.');
define('IP_ACCESS_STATUS_404', 'Неверный путь <b>%TYPE%</b>. Проверьте руководство камеры для корректных путей.');
define('IP_ACCESS_STATUS_401', 'Неверный логин или пароль.');
define('IP_ACCESS_STATUS_303', 'Не удалось разрешить имя хоста.');
define('IP_ACCESS_NCONFIG', 'Не удалось подключиться к камере. Проверьте имя хоста/IP‑адрес.');

#ip ptz presets
define('IPP_NEW', 'Создать набор PTZ‑команд');
define('IPP_DELETE', 'Удалить пресет «');
define('IPP_HEADER', 'Выберите PTZ‑пресет для');
define('IPP_EDIT', 'Редактировать PTZ‑пресет');
define('IPP_EDIT_HELP', 'Заполните команды пресета согласно спецификации производителя. <br /><br />Доступны следующие переменные: <br /> 
							<b>%USERNAME%</b> - Username to access the camera <br />
							<b>%PASSWORD%</b> - Password to access the camera <br />
							<b>%ID%</b> - ID for the command, e.g. position preset ID');
define('IPP_PRESET_NAME', 'Имя пресета');
define('IPP_PRESET_NAME_EX', 'имя пресета');


define('IPP_PRESET_RIGHT', 'Вправо');
define('IPP_PRESET_LEFT', 'Влево');
define('IPP_PRESET_UP', 'Вверх');
define('IPP_PRESET_DOWN', 'Вниз');
define('IPP_PRESET_UPRIGHT', 'Вверх‑вправо');
define('IPP_PRESET_UPLEFT', 'Вверх‑влево');
define('IPP_PRESET_DOWNRIGHT', 'Вниз‑вправо');
define('IPP_PRESET_DOWNLEFT', 'Вниз‑влево');
define('IPP_PRESET_TIGHT', 'Увеличить');
define('IPP_PRESET_TIGHT_EX', 'приблизить');
define('IPP_PRESET_WIDE', 'Уменьшить');
define('IPP_PRESET_WIDE_EX', 'отдалить');
define('IPP_PRESET_FOCUSIN', 'Фокус +');
define('IPP_PRESET_FOCUSOUT', 'Фокус −');
define('IPP_PRESET_PRESET_SAVE', 'Сохранить пресет');
define('IPP_PRESET_PRESET_GO', 'Перейти к пресету');
define('IPP_PRESET_STOP', 'Стоп');
define('IPP_PRESET_STOP_EX', 'остановить движение');
define('IPP_PRESET_NEEDS_STOP', 'Требуется стоп');
define('IPP_PRESET_NEEDS_STOP_EX', 'нужна ли камере команда стоп');
define('IPP_PRESET_HTTP_AUTH', 'Использовать HTTP‑аутентификацию');
define('IPP_PRESET_HTTP_AUTH_EX', 'HTTP‑auth или GET‑параметры');
define('IPP_PRESET_PROTOCOL', 'Протокол/Интерфейс');
define('IPP_PRESET_PORT', 'Порт управления');
define('IPP_PRESET_PORT_EX', 'Укажите порт, если используется нестандартный');
define('IPP_DISABLE_PTZ', 'Отключить PTZ для этой камеры');

#statistics
define('STS_SUBHEADER', 'распределение событий по типу и диапазону дат');
define('STS_HEADER', 'Статистика событий');
define('STS_START_DATE', 'Дата начала');
define('STS_END_DATE', 'Дата окончания');
define('STS_PR_GRP', 'Основная группировка');
define('STS_SC_GRP', 'Вторичная группировка');
define('STS_ET_ALL', 'Все события');
define('STS_ET_M', 'Движение');
define('STS_ET_C', 'Непрерывно');
define('STS_SUBMIT', 'Получить статистику');
define('STS_TOTAL_EVENTS', 'Всего событий <b>%TYPE%</b> за период:');
define('STS_PICKER_HEADER', 'Выберите диапазон дат и типы событий');
define('STS_PERCENTAGE_OF_TOTAL', 'Процент от общего');
define('STS_NUM_EVENTS', 'Количество событий');

#notifications
define('NTF_HEADER', 'Email‑уведомления');
define('NTF_EXISTING', 'Текущие правила');
define('NTF_SUBHEADER', 'настройте правила email‑уведомлений');
define('NTF_ADD_RULE', 'Добавить правило');
define('NTF_EDIT_RULE', 'Сохранить изменения');
define('NTF_ADD_RULE_TITLE', 'Добавить новое правило уведомлений');
define('NTF_ADD_RULE_DATETIME', 'Выберите дни и время для уведомлений');
define('NTF_DAY', 'День недели');
define('NTF_START_TIME', 'Время начала');
define('NTF_END_TIME', 'Время окончания');
define('NTF_CAMERAS', 'выберите камеры');
define('NTF_USERS', 'пользователи для уведомления');
define('NTF_LIMIT', 'Максимальный лимит уведомлений');
define('NTF_LIMIT_LABEL', 'Лимит писем в час [0 — без ограничений]');
define('NTF_NO_RULES', 'Правил уведомлений пока нет');
define('NTF_EXISTING_RULES', 'Действующие правила уведомлений');

define('NTF_E_USERS', 'Выберите как минимум одного пользователя');
define('NTF_E_DAYS', 'Выберите как минимум один день недели');
define('NTF_E_TIME', 'Время начала должно быть раньше времени окончания');
define('NTF_E_CAMS', 'Выберите как минимум одну камеру');

define('NTF_TEMPLATE', 'Отправлять письма пользователям <b>%USERS%</b> <br />в дни <b>%DAYS%</b> <br />между <b>%STIME%</b> и <b>%ETIME%</b> <br />для событий по <b>%CAMERAS%</b>.');
define('NTF_T_TO', 'Отправлять письма');
define('NTF_T_DOW', 'в дни');
define('NTF_T_TIME', 'между часами');
define('NTD_T_CAMS', 'для событий по');
define('NTD_T_LIM', 'писем в час');


define('DW_M', 'Понедельник');
define('DW_T', 'Вторник');
define('DW_W', 'Среда');
define('DW_R', 'Четверг');
define('DW_F', 'Пятница');
define('DW_S', 'Суббота');
define('DW_U', 'Воскресенье');

#backup/restore
define('BACKUP_HEADER', 'Резервное копирование и восстановление базы данных');
define('BACKUP_R_CONF', 'Восстановление базы данных безвозвратно удалит все текущие данные. Настоятельно рекомендуем сделать актуальную резервную копию перед восстановлением.');
define('BACKUP_SUBHEADER', 'создайте резервную копию БД или восстановитесь из сохранённой');
define('BACKUP_B_TITLE', 'Создать резервную копию БД');
define('BACKUP_PWD', 'Введите пароль пользователя базы данных <b>%USERS%</b> для БД <b>%DBNAME%</b>:');
define('BACKUP_B_OPT', 'Дополнительные параметры');
define('BACKUP_B_OPT_NE', 'Не включать данные событий');
define('BACKUP_B_OPT_ND', 'Не включать конфигурацию устройств');
define('BACKUP_B_OPT_NU', 'Не включать записи пользователей');
define('BACKUP_R_TITLE', 'Восстановить базу данных');
define('BACKUP_R_WRONG_FILETYPE', 'Ожидается архив tar сжатый. Загрузите корректный файл резервной копии.');
define('BACKUP_R_NOINFO', 'Не удалось прочитать файл информации о резервной копии. Возможно, архив повреждён или содержит неверные данные. Продолжение может привести к потере данных.');
define('BACKUP_R_LABEL', 'Выберите ранее сохранённую резервную копию:');
define('BACKUP_R_INFO', 'Файлы резервной копии загружены. <br /> <b>Дата копии:</b> %DATE% <br /> <b>Данные событий:</b> %E% <br /><b>Данные устройств:</b> %D%<br /><b>Данные пользователей:</b> %U%');
define('BACKUP_FAILED', 'Не удалось выполнить резервное копирование. Пароль к базе верный?');
define('BACKUP_R_FAILED', 'Не удалось восстановить базу данных. Ошибка MySQL:<br />');
define('BACKUP_R_SUCCESS', 'База данных успешно восстановлена. Перезагрузите страницу.');
define('U_BACKUP', 'Резервная копия');
define('U_UPLOAD', 'Загрузить');
define('U_RESTORE', 'Восстановить');
define('U_CANCEL', 'Отмена');
define('U_INCLUDED', 'Включено');
define('U_NOTINCLUDED', 'Не включено');
define('BACKUP_R_WARN', 'Внимание: все таблицы в резервной копии будут безвозвратно перезаписаны после завершения восстановления. Введите пароль БД для продолжения.');
define('BACKUP_DOWNLOAD', 'Скачать файл резервной копии');
define('BACKUP_B_SUCCESS', 'Создано');

#licensing
define('L_ADD_LICENSE_KEY_TITLE', 'Добавить лицензионный ключ');
define('L_ACTIVATE', 'Активировать');
define('L_PORTS', 'Порты');
define('L_CONFIRM_LICENSE_KEY', 'Подтвердить лицензионный ключ');
define('L_DEACTIVATE', 'Деактивировать');

define('L_CONFIRMCODE_MSG', 'К сожалению, не удалось автоматически подтвердить лицензионный ключ. Проверьте подключение к Интернету и попробуйте снова, либо <a href="mailto:support@bluecherrydvr.com">свяжитесь</a> с Bluecherry для получения авторизации, указав этот уникальный идентификатор: ');
define('L_INVALID_LICENSE', 'Введённый вами лицензионный ключ недействителен. Убедитесь, что вы ввели его точно. Если проблема повторяется, <a href="mailto:support@bluecherrydvr.com">свяжитесь</a> с Bluecherry.');
define('L_INVALID_LICENSE_EXISTS', 'Введённый лицензионный ключ уже используется в этой системе.');
define('L_INVALID_CONFIRMATION', 'Ключ подтверждения недействителен. Убедитесь, что вы ввели его точно. Если проблема повторяется, <a href="mailto:support@bluecherrydvr.com">свяжитесь</a> с Bluecherry.');

define('L_CURRENT',  'Активный лицензионный ключ');
define('L_CONFIRM', 'Подтвердить');
define('L_CONFIRM_DELETE', 'Вы уверены, что хотите деактивировать этот лицензионный ключ?');
define('L_NO_LICENSES', 'У вас нет активного лицензионного ключа.');
define('L_AUTO_RESP_1', 'Такого лицензионного ключа не существует.');
define('L_AUTO_RESP_2', 'Этот ключ уже использовался на другом компьютере. Если вы недавно меняли конфигурацию, <a href="mailto:support@bluecherrydvr.com">свяжитесь с Bluecherry</a> для сброса лицензии.');
define('L_AUTO_RESP_3', 'Слишком много неудачных попыток авторизации за день. <a href="mailto:support@bluecherrydvr.com">Свяжитесь с Bluecherry</a> или попробуйте позже.');
define('L_AUTO_RESP_4', 'Для получения авторизации свяжитесь с Bluecherry по телефону или email.');
define('L_AUTO_RESP_5', 'Возникла ошибка при авторизации лицензии. Попробуйте позже или <a href="mailto:support@bluecherrydvr.com">свяжитесь с Bluecherry</a>.');

define('L_LA_OK', 'Код успеха.');
define('L_LA_FAIL', 'Операция не выполнена.');
define('L_LA_EXPIRED', 'Срок действия лицензии истёк или изменено системное время. Проверьте настройки даты и времени.');
define('L_LA_SUSPENDED', 'Лицензия приостановлена.');
define('L_LA_GRACE_PERIOD_OVER', 'Льготный период для синхронизации сервера истёк.');
define('L_LA_TRIAL_EXPIRED', 'Срок пробной версии истёк или изменено системное время. Проверьте настройки даты и времени.<br>Если они верны, приобретите лицензию и активируйте приложение.');
define('L_LA_LOCAL_TRIAL_EXPIRED', 'Срок локальной пробной версии истёк или изменено системное время. Проверьте настройки даты и времени.');
define('L_LA_RELEASE_UPDATE_AVAILABLE', 'Доступно обновление продукта. Опубликован новый релиз.');
define('L_LA_RELEASE_NO_UPDATE_AVAILABLE', 'Новых обновлений нет. Текущая версия — последняя.');
define('L_LA_E_FILE_PATH', 'Неверный путь к файлу.');
define('L_LA_E_PRODUCT_FILE', 'Некорректный или повреждённый файл продукта.');
define('L_LA_E_PRODUCT_DATA', 'Некорректные данные продукта.');
define('L_LA_E_PRODUCT_ID', 'Неверный идентификатор продукта.');
define('L_LA_E_SYSTEM_PERMISSION', 'Недостаточно системных прав. Возникает при использовании флага LA_SYSTEM без запуска с правами администратора.');
define('L_LA_E_FILE_PERMISSION', 'Нет прав на запись в файл.');
define('L_LA_E_WMIC', 'Не удалось сгенерировать отпечаток.');
define('L_LA_E_TIME', 'Разница между сетевым и системным временем превышает допустимое смещение.');
define('L_LA_E_INET', 'Не удалось подключиться к серверу из‑за сетевой ошибки.');
define('L_LA_E_NET_PROXY', 'Неверный сетевой прокси.');
define('L_LA_E_HOST_URL', 'Неверный адрес хоста Cryptlex.');
define('L_LA_E_BUFFER_SIZE', 'Размер буфера меньше требуемого.');
define('L_LA_E_APP_VERSION_LENGTH', 'Длина версии приложения превышает 256 символов.');
define('L_LA_E_REVOKED', 'Лицензия отозвана.');
define('L_LA_E_LICENSE_KEY', 'Неверный лицензионный ключ.');
define('L_LA_E_LICENSE_TYPE', 'Неверный тип лицензии. Убедитесь, что не используется плавающая лицензия.');
define('L_LA_E_OFFLINE_RESPONSE_FILE', 'Неверный файл ответа офлайн‑активации.');
define('L_LA_E_OFFLINE_RESPONSE_FILE_EXPIRED', 'Срок файла ответа офлайн‑активации истёк.');
define('L_LA_E_ACTIVATION_LIMIT', 'Достигнут лимит активаций лицензии.');
define('L_LA_E_ACTIVATION_NOT_FOUND', 'Активация лицензии удалена на сервере.');
define('L_LA_E_DEACTIVATION_LIMIT', 'Достигнут лимит деактиваций лицензии.');
define('L_LA_E_TRIAL_NOT_ALLOWED', 'Пробная версия недоступна для продукта.');
define('L_LA_E_TRIAL_ACTIVATION_LIMIT', 'Достигнут лимит активаций пробной версии для вашей учётной записи.');
define('L_LA_E_MACHINE_FINGERPRINT', 'Отпечаток устройства изменился после активации.');
define('L_LA_E_METADATA_KEY_LENGTH', 'Длина ключа метаданных более 256 символов.');
define('L_LA_E_METADATA_VALUE_LENGTH', 'Длина значения метаданных более 256 символов.');
define('L_LA_E_ACTIVATION_METADATA_LIMIT', 'Достигнут лимит полей метаданных лицензии');
define('L_LA_E_TRIAL_ACTIVATION_METADATA_LIMIT', 'Достигнут лимит полей метаданных пробной версии.');
define('L_LA_E_METADATA_KEY_NOT_FOUND', 'Ключ метаданных не существует.');
define('L_LA_E_TIME_MODIFIED', 'Системное время было изменено (переведено назад).');
define('L_LA_E_RELEASE_VERSION_FORMAT', 'Неверный формат версии.');
define('L_LA_E_AUTHENTICATION_FAILED', 'Неверный email или пароль.');
define('L_LA_E_METER_ATTRIBUTE_NOT_FOUND', 'Атрибут счётчика не существует.');
define('L_LA_E_METER_ATTRIBUTE_USES_LIMIT_REACHED', 'Достигнут лимит использования атрибута счётчика.');
define('L_LA_E_CUSTOM_FINGERPRINT_LENGTH', 'Длина пользовательского отпечатка устройства менее 64 или более 256 символов.');
define('L_LA_E_VM', 'Приложение запущено внутри виртуальной машины/гипервизора, активация в ВМ запрещена.');
define('L_LA_E_COUNTRY', 'Страна не разрешена.');
define('L_LA_E_IP', 'IP‑адрес не разрешён.');
define('L_LA_E_RATE_LIMIT', 'Достигнут лимит запросов к API, попробуйте позже.');
define('L_LA_E_SERVER', 'Ошибка сервера.');
define('L_LA_E_CLIENT', 'Ошибка клиента.');

define('L_LA_E_BC_SERVER', 'Возникла проблема при подключении к bc‑server.');
define('L_LA_E_TRIAL_ACTIVATED', 'Пробная версия активирована. Осталось дней: ');
define('L_LA_E_LICENSE_ACTIVATED', 'Лицензия успешно активирована.');
define('L_LA_E_TRIAL_ACTIVATE_REQUIRE', 'Пробная версия не была запущена или была изменена, чтобы активировать пробную версию нажмите здесь.<br><form action="/licenses?mode=activate_trial" method="POST"><button class="link-button send-req-form" type="submit" data-func-after="licensingActivateTrial">Активировать пробную версию.</button></form>');
define('L_LA_E_TRIAL_ACTIVATE_SUCCESS', 'Пробная версия успешно активирована.');
define('L_LA_E_UNKNOWN', 'Неизвестная ошибка.');

#Cam specific
define('ACTI_STREAMING_SET_3', 'Ваша камера ACTi была автоматически настроена на RTP поверх UDP.');
define('ACTI_STREAMING_N_SET_3', 'Не удалось автоматически настроить камеру ACTi на RTP поверх UDP. Выполните настройку вручную.');

define('VIVOTEC_SETRAMING_SET', 'Ваша камера Vivotec автоматически настроена на MJPEG на втором кодере.');
define('VIVOTEC_SETRAMING_N_SET', 'Не удалось автоматически настроить камеру Vivotec на MJPEG на втором кодере. Выполните настройку вручную.');

define('L_DELETE', 'удалить');
define('L_EDIT', 'редактировать');
define('L_NONE', 'нет');
define('U_LOADING', 'Загрузка…');
define('L_ENABLE', 'включить');
define('L_DISABLE', 'отключить');
define('DB_SYNC_BUTTON', 'Синхронизировать базу');

define('BETA_LICENSE_WARNING', 'Bluecherry DVR теперь свободное ПО с открытым исходным кодом.<br>');

#ip cam props
define('IPCAM_TCPUDP_LEVEL', 'Транспорт');

#playback
define('PLAYBACK_FILE_NOT_FOUND', 'Файл MKV не найден.');

#webhook
define('WEBHOOK_FORM_HEADING_ADD', 'Добавить Webhook');
define('WEBHOOK_FORM_HEADING_EDIT', 'Редактировать Webhook');
define('WEBHOOK_FORM_FIELD_LABEL', 'Метка');
define('WEBHOOK_FORM_FIELD_URL', 'URL');
define('WEBHOOK_FORM_FIELD_EVENTS', 'События');
define('WEBHOOK_FORM_FIELD_CAMERAS', 'Камеры');
define('WEBHOOK_FORM_FIELD_ENABLED', 'Включено');
define('WEBHOOK_FORM_FIELD_EVENT_MOTION_EVENT', 'Событие движения');
define('WEBHOOK_FORM_FIELD_EVENT_DEVICE_STATE', 'Статус устройства');
define('WEBHOOK_FORM_FIELD_EVENT_SOLO', 'Аналоговая карта захвата Bluecherry (solo)');
define('WEBHOOK_FORM_BUTTON_CREATE', 'Создать');
define('WEBHOOK_FORM_BUTTON_UPDATE', 'Обновить');
define('WEBHOOK_LISTING_HEADER', 'Список Webhook');
define('WEBHOOK_LISTING_TABLE_HEADER_LABEL', 'Метка');
define('WEBHOOK_LISTING_TABLE_HEADER_ACTIONS', 'Действия');
define('WEBHOOK_LISTING_TABLE_LAST_UPDATE', 'Последнее обновление:');
define('WEBHOOK_LISTING_TABLE_BUTTON_EDIT', 'Редактировать');
define('WEBHOOK_LISTING_TABLE_BUTTON_DELETE', 'Удалить');
define('WEBHOOK_LISTING_TABLE_BUTTON_TEST', 'Тест');
define('WEBHOOK_LISTING_TABLE_BUTTON_CREATE_NEW', 'Создать новый');

?>
