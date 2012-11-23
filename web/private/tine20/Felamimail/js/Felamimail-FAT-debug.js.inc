/*!
 * Tine 2.0 - Felamimail 
 * Copyright (c) 2007-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 * http://www.gnu.org/licenses/agpl.html AGPL Version 3
 */
/*
 * Tine 2.0
 * 
 * @package     Felamimail
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 * @copyright   Copyright (c) 2007-2012 Metaways Infosystems GmbH (http://www.metaways.de)
 * 
 * TODO         think about adding a generic felamimail backend with the exception handler
 */
Ext.ns('Tine.Felamimail.Model');

/**
 * @namespace Tine.Felamimail.Model
 * @class Tine.Felamimail.Model.Message
 * @extends Tine.Tinebase.data.Record
 * 
 * Message Record Definition
 */ 
Tine.Felamimail.Model.Message = Tine.Tinebase.data.Record.create([
      { name: 'id' },
      { name: 'account_id' },
      { name: 'subject' },
      { name: 'from_email' },
      { name: 'from_name' },
      { name: 'sender' },
      { name: 'to' },
      { name: 'cc' },
      { name: 'bcc' },
      { name: 'sent',     type: 'date', dateFormat: Date.patterns.ISO8601Long },
      { name: 'received', type: 'date', dateFormat: Date.patterns.ISO8601Long },
      { name: 'flags' },
      { name: 'size' },
      { name: 'body',     defaultValue: undefined },
      { name: 'headers' },
      { name: 'content_type' },
      { name: 'body_content_type' },
      { name: 'structure' },
      { name: 'attachments' },
      { name: 'original_id' },
      { name: 'folder_id' },
      { name: 'note' },
      { name: 'preparedParts' } // contains invitation event record
    ], {
    appName: 'Felamimail',
    modelName: 'Message',
    idProperty: 'id',
    titleProperty: 'subject',
    // ngettext('Message', 'Messages', n);
    recordName: 'Message',
    recordsName: 'Messages',
    containerProperty: 'folder_id',
    // ngettext('Folder', 'Folders', n);
    containerName: 'Folder',
    containersName: 'Folders',
    
    /**
     * check if message has given flag
     * 
     * @param  {String} flag
     * @return {Boolean}
     */
    hasFlag: function(flag) {
        var flags = this.get('flags') || [];
        return flags.indexOf(flag) >= 0;
    },
    
    /**
     * adds given flag to message
     * 
     * @param  {String} flag
     * @return {Boolean} false if flag was already set before, else true
     */
    addFlag: function(flag) {
        if (! this.hasFlag(flag)) {
            var flags = Ext.unique(this.get('flags'));
            flags.push(flag);
            
            this.set('flags', flags);
            return true;
        }
        
        return false;
    },
    
    /**
     * check if body has been fetched
     * 
     * @return {Boolean}
     */
    bodyIsFetched: function() {
        return this.get('body') !== undefined;
    },
    
    /**
     * clears given flag from message
     * 
     * @param {String} flag
     * @return {Boolean} false if flag was not set before, else true
     */
    clearFlag: function(flag) {
        if (this.hasFlag(flag)) {
            var flags = Ext.unique(this.get('flags'));
            flags.remove(flag);
            
            this.set('flags', flags);
            return true;
        }
        
        return false;
    }
});

/**
 * get default message data
 * 
 * @return {Object}
 */
Tine.Felamimail.Model.Message.getDefaultData = function() {
    var autoAttachNote = Tine.Felamimail.registry.get('preferences').get('autoAttachNote');
    
    return {
        note: autoAttachNote,
        content_type: 'text/html'
    };
};

/**
 * get filtermodel for messages
 * 
 * @namespace Tine.Felamimail.Model
 * @static
 * @return {Object} filterModel definition
 */ 
Tine.Felamimail.Model.Message.getFilterModel = function() {
    var app = Tine.Tinebase.appMgr.get('Felamimail');
    
    return [
        {filtertype: 'tine.felamimail.folder.filtermodel', app: app},
        {label: app.i18n._('Subject/From'),field: 'query',         operators: ['contains']},
        {label: app.i18n._('Subject'),     field: 'subject',       operators: ['contains']},
        {label: app.i18n._('From (Email)'),field: 'from_email',    operators: ['contains']},
        {label: app.i18n._('From (Name)'), field: 'from_name',     operators: ['contains']},
        {label: app.i18n._('To'),          field: 'to',            operators: ['contains']},
        {label: app.i18n._('Cc'),          field: 'cc',            operators: ['contains']},
        {label: app.i18n._('Bcc'),         field: 'bcc',           operators: ['contains']},
        {label: app.i18n._('Flags'),       field: 'flags',         filtertype: 'tinebase.multiselect', app: app, multiselectFieldConfig: {
            valueStore: Tine.Felamimail.loadFlagsStore()
        }},
        {label: app.i18n._('Received'),    field: 'received',      valueType: 'date', pastOnly: true}
    ];
};

/**
 * @namespace Tine.Felamimail
 * @class Tine.Felamimail.messageBackend
 * @extends Tine.Tinebase.data.RecordProxy
 * 
 * Message Backend
 * 
 * TODO make clear/addFlags send filter as param instead of array of ids
 */ 
Tine.Felamimail.messageBackend = new Tine.Tinebase.data.RecordProxy({
    appName: 'Felamimail',
    modelName: 'Message',
    recordClass: Tine.Felamimail.Model.Message,
    
    /**
     * move messsages to folder
     *
     * @param  array $filterData filter data
     * @param  string $targetFolderId
     * @return  {Number} Ext.Ajax transaction id
     */
    moveMessages: function(filter, targetFolderId, options) {
        options = options || {};
        options.params = options.params || {};
        
        var p = options.params;
        
        p.method = this.appName + '.moveMessages';
        p.filterData = filter;
        p.targetFolderId = targetFolderId;
        
        options.beforeSuccess = function(response) {
            return [Tine.Felamimail.folderBackend.recordReader(response)];
        };
        
        // increase timeout as this can take a longer (5 minutes)
        options.timeout = 300000;
        
        return this.doXHTTPRequest(options);
    },
    
    /**
     * fetches body and additional headers (which are needed for the preview panel) into given message
     * 
     * @param {Message} message
     * @param {Function|Object} callback (NOTE: this has NOTHING to do with standard Ext request callback fn)
     */
    fetchBody: function(message, callback) {
        return this.loadRecord(message, {
            timeout: 120000, // 2 minutes
            scope: this,
            success: function(response, options) {
                var msg = this.recordReader({responseText: Ext.util.JSON.encode(response.data)});
                // NOTE: Flags from the server might be outdated, so we skip them
                Ext.copyTo(message.data, msg.data, Tine.Felamimail.Model.Message.getFieldNames().remove('flags'));
                if (Ext.isFunction(callback)) {
                    callback(message);
                } else if (callback.success) {
                    Ext.callback(callback.success, callback.scope, [message]);
                }
            },
            failure: function(exception) {
                if (callback.failure) {
                    Ext.callback(callback.failure, callback.scope, [exception]);
                } else {
                    this.handleRequestException(exception);
                }
            }
        });
    },
    
    /**
     * saves a message into a folder
     * 
     * @param   {Ext.data.Record} record
     * @param   {String} folderName
     * @param   {Object} options
     * @return  {Number} Ext.Ajax transaction id
     * @success {Ext.data.Record}
     */
    saveInFolder: function(record, folderName, options) {
        options = options || {};
        options.params = options.params || {};
        options.beforeSuccess = function(response) {
            return [this.recordReader(response)];
        };
        
        var p = options.params;
        p.method = this.appName + '.saveMessageInFolder';
        p.recordData = record.data;
        p.folderName = folderName;
        
        // increase timeout as this can take a longer (5 minutes)
        options.timeout = 300000;
        
        return this.doXHTTPRequest(options);
    },

    
    /**
     * add given flags to given messages
     *
     * @param  {String/Array} ids
     * @param  {String/Array} flags
     */
    addFlags: function(ids, flags, options)
    {
        options = options || {};
        options.params = options.params || {};
        
        var p = options.params;
        
        p.method = this.appName + '.addFlags';
        p.filterData = ids;
        p.flags = flags;
        
        // increase timeout as this can take a longer (5 minutes)
        options.timeout = 300000;
        
        return this.doXHTTPRequest(options);
    },
    
    /**
     * clear given flags from given messages
     *
     * @param  {String/Array} ids
     * @param  {String/Array} flags
     */
    clearFlags: function(ids, flags, options)
    {
        options = options || {};
        options.params = options.params || {};
        
        var p = options.params;
        
        p.method = this.appName + '.clearFlags';
        p.filterData = ids;
        p.flags = flags;
        
        // increase timeout as this can take a longer (5 minutes)
        options.timeout = 300000;
        
        return this.doXHTTPRequest(options);
    },
    
    /**
     * exception handler for this proxy
     * 
     * @param {Tine.Exception} exception
     */
    handleRequestException: function(exception) {
        Tine.Felamimail.handleRequestException(exception);
    }
});


/**
 * @namespace Tine.Felamimail.Model
 * @class Tine.Felamimail.Model.Account
 * @extends Tine.Tinebase.data.Record
 * 
 * Account Record Definition
 */ 
Tine.Felamimail.Model.Account = Tine.Tinebase.data.Record.create(Tine.Tinebase.Model.genericFields.concat([
    { name: 'id' },
    { name: 'original_id' }, // client only, used in message compose dialog for accounts combo
    { name: 'user_id' },
    { name: 'name' },
    { name: 'type' },
    { name: 'user' },
    { name: 'host' },
    { name: 'email' },
    { name: 'password' },
    { name: 'from' },
    { name: 'organization' },
    { name: 'port' },
    { name: 'ssl' },
    { name: 'imap_status', defaultValue: 'success'}, // client only {success|failure}
    { name: 'sent_folder' },
    { name: 'trash_folder' },
    { name: 'drafts_folder' },
    { name: 'templates_folder' },
    { name: 'has_children_support', type: 'bool' },
    { name: 'delimiter' },
    { name: 'display_format' },
    { name: 'ns_personal' },
    { name: 'ns_other' },
    { name: 'ns_shared' },
    { name: 'signature' },
    { name: 'signature_position' },
    { name: 'smtp_port' },
    { name: 'smtp_hostname' },
    { name: 'smtp_auth' },
    { name: 'smtp_ssl' },
    { name: 'smtp_user' },
    { name: 'smtp_password' },
    { name: 'sieve_hostname' },
    { name: 'sieve_port' },
    { name: 'sieve_ssl' },
    { name: 'sieve_vacation_active', type: 'bool' },
    { name: 'all_folders_fetched', type: 'bool', defaultValue: false } // client only
]), {
    appName: 'Felamimail',
    modelName: 'Account',
    idProperty: 'id',
    titleProperty: 'name',
    // ngettext('Account', 'Accounts', n);
    recordName: 'Account',
    recordsName: 'Accounts',
    containerProperty: 'container_id',
    // ngettext('Email Accounts', 'Email Accounts', n);
    containerName: 'Email Accounts',
    containersName: 'Email Accounts',
    
    /**
     * @type Object
     */
    lastIMAPException: null,
    
    /**
     * get the last IMAP exception
     * 
     * @return {Object}
     */
    getLastIMAPException: function() {
        return this.lastIMAPException;
    },
    
    /**
     * returns sendfolder id
     * -> needed as trash is saved as globname :(
     */
    getSendFolderId: function() {
        var app = Ext.ux.PopupWindowMgr.getMainWindow().Tine.Tinebase.appMgr.get('Felamimail'),
            sendName = this.get('sent_folder'),
            accountId = this.id,
            send = sendName ? app.getFolderStore().queryBy(function(record) {
                return record.get('account_id') === accountId && record.get('globalname') === sendName;
            }, this).first() : null;
            
        return send ? send.id : null;
    },
    
    /**
     * returns trashfolder id
     * -> needed as trash is saved as globname :(
     */
    getTrashFolderId: function() {
        var app = Ext.ux.PopupWindowMgr.getMainWindow().Tine.Tinebase.appMgr.get('Felamimail'),
            trashName = this.get('trash_folder'),
            accountId = this.id,
            trash = trashName ? app.getFolderStore().queryBy(function(record) {
                return record.get('account_id') === accountId && record.get('globalname') === trashName;
            }, this).first() : null;
            
        return trash ? trash.id : null;
    },
    
    /**
     * set or clear IMAP exception and update imap_state
     * 
     * @param {Object} exception
     */
    setLastIMAPException: function(exception) {
        this.lastIMAPException = exception;
        this.set('imap_status', exception ? 'failure' : 'success');
        this.commit();
    }
});

/**
 * get default data for account
 * 
 * @return {Object}
 */
Tine.Felamimail.Model.Account.getDefaultData = function() {
    var defaults = (Tine.Felamimail.registry.get('defaults')) 
        ? Tine.Felamimail.registry.get('defaults')
        : {};
        
    var currentUserDisplayName = Tine.Tinebase.registry.get('currentAccount').accountDisplayName;
    
    return {
        from: currentUserDisplayName,
        host: (defaults.host) ? defaults.host : '',
        port: (defaults.port) ? defaults.port : 143,
        smtp_hostname: (defaults.smtp && defaults.smtp.hostname) ? defaults.smtp.hostname : '',
        smtp_port: (defaults.smtp && defaults.smtp.port) ? defaults.smtp.port : 25,
        smtp_ssl: (defaults.smtp && defaults.smtp.ssl) ? defaults.smtp.ssl : 'none',
        sieve_port: 2000,
        sieve_ssl: 'none',
        signature: 'Sent with love from the new tine 2.0 email client ...<br/>'
            + 'Please visit <a href="http://tine20.org">http://tine20.org</a>',
        sent_folder: (defaults.sent_folder) ? defaults.sent_folder : 'Sent',
        trash_folder: (defaults.trash_folder) ? defaults.trash_folder : 'Trash'
    };
};

/**
 * @namespace Tine.Felamimail
 * @class Tine.Felamimail.accountBackend
 * @extends Tine.Tinebase.data.RecordProxy
 * 
 * Account Backend
 */ 
Tine.Felamimail.accountBackend = new Tine.Tinebase.data.RecordProxy({
    appName: 'Felamimail',
    modelName: 'Account',
    recordClass: Tine.Felamimail.Model.Account
});

/**
 * @namespace Tine.Felamimail.Model
 * @class Tine.Felamimail.Model.Record
 * @extends Ext.data.Record
 * 
 * Folder Record Definition
 */ 
Tine.Felamimail.Model.Folder = Tine.Tinebase.data.Record.create([
      { name: 'id' },
      { name: 'localname' },
      { name: 'globalname' },
      { name: 'path' }, // /accountid/folderid/...
      { name: 'parent' },
      { name: 'parent_path' }, // /accountid/folderid/...
      { name: 'account_id' },
      { name: 'has_children',       type: 'bool' },
      { name: 'is_selectable',      type: 'bool' },
      { name: 'system_folder',      type: 'bool' },
      { name: 'imap_status' },
      { name: 'imap_timestamp',     type: 'date', dateFormat: Date.patterns.ISO8601Long },
      { name: 'imap_uidvalidity',   type: 'int' },
      { name: 'imap_totalcount',    type: 'int' },
      { name: 'cache_status' },
      { name: 'cache_recentcount',  type: 'int' },
      { name: 'cache_totalcount',   type: 'int' },
      { name: 'cache_unreadcount',  type: 'int' },
      { name: 'cache_timestamp',    type: 'date', dateFormat: Date.patterns.ISO8601Long  },
      { name: 'cache_job_actions_est',     type: 'int' },
      { name: 'cache_job_actions_done',         type: 'int' },
      { name: 'quota_usage',         type: 'int' },
      { name: 'quota_limit',         type: 'int' },
      { name: 'client_access_time', type: 'date', dateFormat: Date.patterns.ISO8601Long  }, // client only {@see Tine.Felamimail.folderBackend#updateMessageCache}
      { name: 'unread_children', type: 'Array', defaultValue: [] } // client only / array of unread child ids
], {
    // translations for system folders:
    // _('INBOX') _('Drafts') _('Sent') _('Templates') _('Junk') _('Trash')

    appName: 'Felamimail',
    modelName: 'Folder',
    idProperty: 'id',
    titleProperty: 'localname',
    // ngettext('Folder', 'Folders', n);
    recordName: 'Folder',
    recordsName: 'Folders',
    // ngettext('record list', 'record lists', n);
    containerName: 'Folder list',
    containersName: 'Folder lists',
    
    /**
     * is this folder the currently selected folder
     * 
     * @return {Boolean}
     */
    isCurrentSelection: function() {
        if (Tine.Tinebase.appMgr.get(this.appName).getMainScreen().getTreePanel()) {
            // get active node
            var node = Tine.Tinebase.appMgr.get(this.appName).getMainScreen().getTreePanel().getSelectionModel().getSelectedNode();
            if (node && node.attributes.folder_id) {
                return node.id == this.id;
            }
        }
        
        return false;
    },
    
    /**
     * is this folder an inbox?
     * 
     * @return {Boolean}
     */
    isInbox: function() {
        return Ext.util.Format.lowercase(this.get('localname')) === 'inbox';
    },
    
    /**
     * returns true if current folder needs an update
     */
    needsUpdate: function(updateInterval) {
        if (! this.get('is_selectable')) {
            return false;
        }
        
        var timestamp = this.get('client_access_time');
        return this.get('cache_status') !== 'complete' || ! Ext.isDate(timestamp) || timestamp.getElapsed() > updateInterval;
    }
});

/**
 * @namespace Tine.Felamimail
 * @class Tine.Felamimail.folderBackend
 * @extends Tine.Tinebase.data.RecordProxy
 * 
 * Folder Backend
 */ 
Tine.Felamimail.folderBackend = new Tine.Tinebase.data.RecordProxy({
    appName: 'Felamimail',
    modelName: 'Folder',
    recordClass: Tine.Felamimail.Model.Folder,
    
    /**
     * update message cache of given folder for given execution time and sets the client_access_time
     * 
     * @param   {String} folderId
     * @param   {Number} executionTime (seconds)
     * @return  {Number} Ext.Ajax transaction id
     */
    updateMessageCache: function(folderId, executionTime, options) {
        options = options || {};
        options.params = options.params || {};
        
        var p = options.params;
        
        p.method = this.appName + '.updateMessageCache';
        p.folderId = folderId;
        p.time = executionTime;
        
        options.beforeSuccess = function(response) {
            var folder = this.recordReader(response);
            folder.set('client_access_time', new Date());
            return [folder];
        };
        
        // give 5 times more before timeout
        options.timeout = executionTime * 5000;
        
        return this.doXHTTPRequest(options);
    },
    
    /**
     * exception handler for this proxy
     * 
     * @param {Tine.Exception} exception
     */
    handleRequestException: function(exception) {
        Tine.Felamimail.handleRequestException(exception);
    }
});

/**
 * @namespace Tine.Felamimail.Model
 * @class Tine.Felamimail.Model.Vacation
 * @extends Tine.Tinebase.data.Record
 * 
 * Vacation Record Definition
 */ 
Tine.Felamimail.Model.Vacation = Tine.Tinebase.data.Record.create(Tine.Tinebase.Model.genericFields.concat([
    { name: 'id' },
    { name: 'reason' },
    { name: 'enabled', type: 'boolean'},
    { name: 'days' },
    { name: 'start_date', type: 'date' },
    { name: 'end_date', type: 'date' },
    { name: 'contact_ids' },
    { name: 'template_id' },
    { name: 'signature' },
    { name: 'mime' }
]), {
    appName: 'Felamimail',
    modelName: 'Vacation',
    idProperty: 'id',
    titleProperty: 'id',
    // ngettext('Vacation', 'Vacations', n);
    recordName: 'Vacation',
    recordsName: 'Vacations',
    //containerProperty: 'container_id',
    // ngettext('record list', 'record lists', n);
    containerName: 'Vacation list',
    containersName: 'Vacation lists'    
});

/**
 * get default data for vacation
 * 
 * @return {Object}
 */
Tine.Felamimail.Model.Vacation.getDefaultData = function() {
    return {
        days: 7,
        mime: 'multipart/alternative'
    };
};

/**
 * @namespace Tine.Felamimail
 * @class Tine.Felamimail.vacationBackend
 * @extends Tine.Tinebase.data.RecordProxy
 * 
 * Vacation Backend
 */ 
Tine.Felamimail.vacationBackend = new Tine.Tinebase.data.RecordProxy({
    appName: 'Felamimail',
    modelName: 'Vacation',
    recordClass: Tine.Felamimail.Model.Vacation,
    
    /**
     * exception handler for this proxy
     * 
     * @param {Tine.Exception} exception
     */
    handleRequestException: function(exception) {
        Tine.Felamimail.handleRequestException(exception);
    }
});

/**
 * @namespace Tine.Felamimail.Model
 * @class Tine.Felamimail.Model.Rule
 * @extends Tine.Tinebase.data.Record
 * 
 * Rule Record Definition
 */ 
Tine.Felamimail.Model.Rule = Tine.Tinebase.data.Record.create(Tine.Tinebase.Model.genericFields.concat([
    { name: 'id', sortType: function(value) {
        // should be sorted as int
        return parseInt(value, 10);
    }
    },
    { name: 'action_type' },
    { name: 'action_argument' },
    { name: 'enabled', type: 'boolean'},
    { name: 'conditions' },
    { name: 'account_id' }
]), {
    appName: 'Felamimail',
    modelName: 'Rule',
    idProperty: 'id',
    titleProperty: 'id',
    // ngettext('Rule', 'Rules', n);
    recordName: 'Rule',
    recordsName: 'Rules',
    // ngettext('record list', 'record lists', n);
    containerName: 'Rule list',
    containersName: 'Rule lists'    
});

/**
 * get default data for rules
 * 
 * @return {Object}
 */
Tine.Felamimail.Model.Rule.getDefaultData = function() {
    return {
        enabled: true,
        conditions: [{
            test: 'address',
            header: 'from',
            comperator: 'contains',
            key: ''
        }],
        action_type: 'fileinto',
        action_argument: ''
    };
};

/**
 * @namespace Tine.Felamimail
 * @class Tine.Felamimail.rulesBackend
 * @extends Tine.Tinebase.data.RecordProxy
 * 
 * Rule Backend
 */ 
Tine.Felamimail.rulesBackend = new Tine.Tinebase.data.RecordProxy({
    appName: 'Felamimail',
    modelName: 'Rule',
    recordClass: Tine.Felamimail.Model.Rule,
    
    /**
     * searches all (lightweight) records matching filter
     * 
     * @param   {Object} filter accountId
     * @param   {Object} paging
     * @param   {Object} options
     * @return  {Number} Ext.Ajax transaction id
     * @success {Object} root:[records], totalcount: number
     */
    searchRecords: function(filter, paging, options) {
        options = options || {};
        options.params = options.params || {};
        var p = options.params;
        
        p.method = this.appName + '.get' + this.modelName + 's';
        p.accountId = filter;
        
        options.beforeSuccess = function(response) {
            return [this.jsonReader.read(response)];
        };
        
        // increase timeout as this can take a longer (1 minute)
        options.timeout = 60000;
        
        return this.doXHTTPRequest(options);
    },
    
    /**
     * save sieve rules
     *
     * @param  {String}     accountId
     * @param  {Array}      rules
     * @param  {Object}     options
     */
    saveRules: function(accountId, rules, options)
    {
        options = options || {};
        options.params = options.params || {};
        
        var p = options.params;
        
        p.method = this.appName + '.saveRules';
        p.accountId = accountId;
        p.rulesData = rules;
        
        return this.doXHTTPRequest(options);
    },

    /**
     * saves a single record
     * 
     * NOTE: Single rule records can't be saved
     * 
     * @param   {Ext.data.Record} record
     * @param   {Object} options
     * @return  {Number} Ext.Ajax transaction id
     * @success {Ext.data.Record}
     */
    saveRecord: function(record, options, additionalArguments) {
        // does nothing
    },
    
    /**
     * exception handler for this proxy
     * 
     * @param {Tine.Exception} exception
     */
    handleRequestException: function(exception) {
        Tine.Felamimail.handleRequestException(exception);
    }
});

/**
 * @namespace Tine.Felamimail.Model
 * @class Tine.Felamimail.Model.Flag
 * @extends Tine.Tinebase.data.Record
 * 
 * Flag Record Definition
 */ 
Tine.Felamimail.Model.Flag = Tine.Tinebase.data.Record.create(Tine.Tinebase.Model.genericFields.concat([
    { name: 'id' },
    { name: 'name' }
]), {
    appName: 'Felamimail',
    modelName: 'Flag',
    idProperty: 'id',
    titleProperty: 'id',
    // ngettext('Flag', 'Flags', n);
    recordName: 'Flag',
    recordsName: 'Flags',
    // ngettext('Flag list', 'Flag lists', n);
    containerName: 'Flag list',
    containersName: 'Flag lists'    
});
/*
 * Tine 2.0
 * 
 * @package     Felamimail
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 * @copyright   Copyright (c) 2011-2012 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */
 
Ext.ns('Tine.Felamimail');

/**
 * @namespace   Tine.Felamimail
 * @class       Tine.Felamimail.GridPanelHook
 * 
 * <p>Felamimail Gridpanel Hook</p>
 * <p>
 * </p>
 * 
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * 
 * @constructor
 */
Tine.Felamimail.GridPanelHook = function(config) {
    Ext.apply(this, config);

    Tine.log.info('Tine.Felamimail.GridPanelHook::Initialising Felamimail ' + this.foreignAppName + ' hooks.');
    
    // NOTE: due to the action updater this action is bound the the adb grid only!
    this.composeMailAction = new Ext.Action({
        actionType: 'add',
        text: this.app.i18n._('Compose email'),
        iconCls: this.app.getIconCls(),
        disabled: true,
        scope: this,
        actionUpdater: this.updateAction,
        handler: this.onComposeEmail,
        listeners: {
            scope: this,
            render: this.onRender
        }
    });
    
    this.composeMailBtn = Ext.apply(new Ext.Button(this.composeMailAction), {
        scale: 'medium',
        rowspan: 2,
        iconAlign: 'top'
    });
    
    // register in toolbar + contextmenu
    Ext.ux.ItemRegistry.registerItem(this.foreignAppName + '-GridPanel-ActionToolbar-leftbtngrp', this.composeMailBtn, 30);
    Ext.ux.ItemRegistry.registerItem(this.foreignAppName + '-GridPanel-ContextMenu', this.composeMailAction, 80);
};

Ext.apply(Tine.Felamimail.GridPanelHook.prototype, {
    
    /**
     * @property app
     * @type Tine.Felamimail.Application
     * @private
     */
    app: null,
    
    /**
     * foreign application name
     * @type String
     */
    foreignAppName: null,
    
    /**
     * @property composeMailAction
     * @type Tine.widgets.ActionUpdater
     * @private
     */
    composeMailAction: null,
    
    /**
     * @property composeMailBtn
     * @type Ext.Button
     * @private
     */
    composeMailBtn: null,
    
    /**
     * @property gridPanel
     * @type Tine.Addressbook.gridPanel
     * @private
     */
    gridPanel: null,
    
    contactInRelation: false,
    relationType: null,
    
    /**
     * get addressbook contact grid panel
     */
    getGridPanel: function() {
        if (! this.gridPanel) {
            this.gridPanel = Tine.Tinebase.appMgr.get(this.foreignAppName).getMainScreen().getCenterPanel();
        }
        
        return this.gridPanel;
    },
    
    /**
     * return mail addresses of given contacts 
     * 
     * @param {Array} contacts
     * @param {String} prefered
     * @return {Array}
     */
    getMailAddresses: function(records) {
        var mailAddresses = [];
        
        Ext.each(records, function(record) {
            var contact = null;
            if (this.contactInRelation && record.get('relations')) {
                Ext.each(record.get('relations'), function(relation) {
                    if (relation.type === this.relationType) {
                       this.addMailFromContact(mailAddresses, relation.related_record);
                    }
                }, this);
            } else {
                this.addMailFromContact(mailAddresses, record);
            }
            
        }, this);
        
        Tine.log.debug('Tine.Felamimail.GridPanelHook::getMailAddresses - Got ' + mailAddresses.length + ' email addresses.');
        if (mailAddresses.length > 0) {
            Tine.log.debug(mailAddresses);
        }
        
        return mailAddresses;
    },
    
    /**
     * add mail address from contact (if available) and add it to mailAddresses array
     * 
     * @param {Array} mailAddresses
     * @param {Tine.Addressbook.Model.Contact|Object} contact
     */
    addMailFromContact: function(mailAddresses, contact) {
        if (! contact) {
            return;
        }
        if (! Ext.isFunction(contact.beginEdit)) {
            contact = new Tine.Addressbook.Model.Contact(contact);
        }
        
        var mailAddress = (contact.getPreferedEmail()) ? Tine.Felamimail.getEmailStringFromContact(contact) : null;
        
        if (mailAddress) {
            mailAddresses.push(mailAddress);
        }
    },
    
    /**
     * compose an email to selected contacts
     * 
     * @param {Button} btn 
     */
    onComposeEmail: function(btn) {
        var sm = this.getGridPanel().grid.getSelectionModel(),
            mailAddresses = sm.isFilterSelect ? null : this.getMailAddresses(this.getGridPanel().grid.getSelectionModel().getSelections());

        var popupWindow = Tine.Felamimail.MessageEditDialog.openWindow({
            selectionFilter: sm.isFilterSelect ? Ext.encode(sm.getSelectionFilter()) : null,
            mailAddresses: mailAddresses ? Ext.encode(mailAddresses) : null
        });
    },

    
    /**
     * add to action updater the first time we render
     */
    onRender: function() {
        var actionUpdater = this.getGridPanel().actionUpdater,
            registeredActions = actionUpdater.actions;
            
        if (registeredActions.indexOf(this.composeMailAction) < 0) {
            actionUpdater.addActions([this.composeMailAction]);
        }
    },
    
    /**
     * updates compose button
     * 
     * @param {Ext.Action} action
     * @param {Object} grants grants sum of grants
     * @param {Object} records
     */
    updateAction: function(action, grants, records) {
        var sm = this.getGridPanel().grid.getSelectionModel();
        action.setDisabled(this.getMailAddresses(sm.getSelections()).length == 0);
    }
});
/*
 * Tine 2.0
 * 
 * @package     Felamimail
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2009-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */
 
Ext.namespace('Tine.Felamimail');

/**
 * @namespace   Tine.Felamimail
 * @class       Tine.Felamimail.FolderStore
 * @extends     Ext.data.Store
 * 
 * <p>Felamimail folder store</p>
 * <p>
 * </p>
 * 
 * @author      Philipp Schuele <p.schuele@metaways.de>
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * 
 * @param       {Object} config
 * 
 * @constructor
 * Create a new  Tine.Felamimail.FolderStore
 */
Tine.Felamimail.FolderStore = function(config) {
    config = config || {};
    Ext.apply(this, config);
    
    this.reader = Tine.Felamimail.folderBackend.getReader();
    this.queriesPending = [];
    this.queriesDone = [];

    Tine.Felamimail.FolderStore.superclass.constructor.call(this);
    
    this.on('load', this.onStoreLoad, this);
    this.on('add', this.onStoreAdd, this);
    this.on('loadexception', this.onStoreLoadException, this);
};

Ext.extend(Tine.Felamimail.FolderStore, Ext.data.Store, {
    
    fields: Tine.Felamimail.Model.Folder,
    proxy: Tine.Felamimail.folderBackend,
    
    /**
     * @property queriesDone
     * @type Array
     */
    queriesDone: null,
    
    /**
     * @property queriesPending
     * @type Array
     */
    queriesPending: null,
    
    /**
     * async query
     */
    asyncQuery: function(field, value, callback, args, scope, store) {
        var result = null,
            key = store.getKey(field, value);
        
        Tine.log.info('Tine.Felamimail.FolderStore.asyncQuery: ' + key);
        
        if (store.queriesDone.indexOf(key) >= 0) {
            Tine.log.debug('result already loaded -> directly query store');
            // we need regexp here because query returns all records with path that begins with the value string otherwise
            var valueReg = new RegExp(value + '$');
            result = store.query(field, valueReg);
            args.push(result);
            callback.apply(scope, args);
        } else if (store.queriesPending.indexOf(key) >= 0) {
            Tine.log.debug('result not in store yet, but async query already running -> wait a bit');
            this.asyncQuery.defer(2500, this, [field, value, callback, args, scope, store]);
        } else {
            Tine.log.debug('result is requested the first time -> fetch from server');
            var accountId = value.match(/^\/([a-z0-9]*)/i)[1],
                folderIdMatch = value.match(/[a-z0-9]+\/([a-z0-9]*)$/i),
                folderId = (folderIdMatch) ? folderIdMatch[1] : null,
                folder = folderId ? store.getById(folderId) : null;
            
            if (folderId && ! folder) {
                Tine.log.warn('folder ' + folderId + ' not found -> performing no query at all');
                callback.apply(scope, args);
                return;
            }
            
            store.queriesPending.push(key);
            store.load({
                path: value,
                params: {filter: [
                    {field: 'account_id', operator: 'equals', value: accountId},
                    {field: 'globalname', operator: 'equals', value: (folder !== null) ? folder.get('globalname') : ''}
                ]},
                callback: function () {
                    store.queriesDone.push(key);
                    store.queriesPending.remove(key);
                    
                    // query store again (it should have the new folders now) and call callback function to add nodes
                    result = store.query(field, value);
                    args.push(result);
                    callback.apply(scope, args);
                },
                add: true
            });
        }
    },
    
    /**
     * on store load exception
     * 
     * @param {Tine.Tinebase.data.RecordProxy} proxy
     * @param {String} type
     * @param {Object} error
     * @param {Object} options
     * 
     * TODO remove loading class / remove from queriesDone?
     */
    onStoreLoadException: function(proxy, type, error, options) {
        //var node = options.params.path
        //node.getUI().removeClass("x-tree-node-loading");

        Tine.Felamimail.handleRequestException(error);
    },
    
    
    /**
     * check if query has already loaded or is loading
     * 
     * @param {String} field
     * @param {String} value
     * @return {boolean}
     */
    isLoadedOrLoading: function(field, value) {
        var key = this.getKey(field, value),
            result = false;
        
        result = (this.queriesDone.indexOf(key) >= 0 || this.queriesPending.indexOf(key) >= 0);
        
        return result;
    },
    
    /**
     * get key to store query 
     * 
     * @param  {string} field
     * @param  {mixed} value
     * @return {string}
     */
    getKey: function(field, value) {
        return field + ' -> ' + value;
    },
    
    /**
     * load event handler
     * 
     * @param {Tine.Felamimail.FolderStore} store
     * @param {Tine.Felamimail.Model.Folder} records
     * @param {Object} options
     */
    onStoreLoad: function(store, records, options) {
        this.computePaths(records, options.path);
    },
    
    /**
     * add event handler
     * 
     * @param {Tine.Felamimail.FolderStore} store
     * @param {Tine.Felamimail.Model.Folder} records
     * @param {Integer} index
     */
    onStoreAdd: function(store, records, index) {
        this.computePaths(records, null);
    },

    /**
     * compute paths for folder records
     * 
     * @param {Tine.Felamimail.Model.Folder} records
     * @param {String|null} parentPath
     */
    computePaths: function(records, givenParentPath) {
        var parentPath, path;
        Ext.each(records, function(record) {
            if (givenParentPath === null) {
                var parent = this.getParent(record);
                parentPath = (parent) ? parent.get('path') : '/' + record.get('account_id');
            } else {
                parentPath = givenParentPath;
            }
            path = parentPath + '/' + record.id;
            
            if (record.get('parent_path') != parentPath || record.get('path') != path) {
                record.beginEdit();
                record.set('parent_path', parentPath);
                record.set('path', path);
                record.endEdit();
            }
        }, this);
    },
    
    /**
     * resets the query and removes all records that match it
     * 
     * @param {String} field
     * @param {String} value
     */
    resetQueryAndRemoveRecords: function(field, value) {
        this.queriesPending.remove(this.getKey(field, value));
        var toRemove = this.query(field, value);
        toRemove.each(function(record) {
            this.remove(record);
            this.queriesDone.remove(this.getKey(field, record.get(field)));
        }, this);
    },
    
    /**
     * update folder in this store
     * 
     * NOTE: parent_path and path are computed onLoad and must be preserved
     * 
     * @param {Array/Tine.Felamimail.Model.Folder} update
     * @return {Tine.Felamimail.Model.Folder}
     */
    updateFolder: function(update) {
        if (Ext.isArray(update)) {
            Ext.each(update, function(u) {this.updateFolder.call(this, u)}, this);
            return;
        }
        
        var folder = this.getById(update.id);
        
        if (folder) {
            folder.beginEdit();
            Ext.each(Tine.Felamimail.Model.Folder.getFieldNames(), function(f) {
                if (! f.match('path')) {
                    folder.set(f, update.get(f));
                }
            }, this);
            folder.endEdit();
            folder.commit();
            return folder;
        }
    },
    
    /**
     * get parent folder
     * 
     * @param {Tine.Felamimail.Model.Folder} folder
     * @return {Tine.Felamimail.Model.Folder|null}
     */
    getParent: function(folder) {
        var result = this.queryBy(function(record, id) {
            if (record.get('account_id') == folder.get('account_id') && record.get('globalname') == folder.get('parent')) {
                return true;
            }
        });
        
        return result.first() || null;
    }    
});

/*
 * Tine 2.0
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2010-2012 Metaways Infosystems GmbH (http://www.metaways.de)
 */
Ext.ns('Tine.Felamimail');

/**
 * folder select trigger field
 * 
 * @namespace   Tine.widgets.container
 * @class       Tine.Felamimail.FolderSelectTriggerField
 * @extends     Ext.form.ComboBox
 * @author      Philipp Schuele <p.schuele@metaways.de>
 * 
 */
Tine.Felamimail.FolderSelectTriggerField = Ext.extend(Ext.form.TriggerField, {
    
    triggerClass: 'x-form-search-trigger',
    account: null,
    allAccounts: false,
    
    /**
     * onTriggerClick
     * open ext window with (folder-)select panel that fires event on select
     * 
     * @param e
     */
    onTriggerClick: function(e) {
        if (! this.disabled && (this.account && this.account.id !== 0) || this.allAccounts) {
            this.selectPanel = Tine.Felamimail.FolderSelectPanel.openWindow({
                account: this.account,
                allAccounts: this.allAccounts,
                listeners: {
                    // NOTE: scope has to be first item in listeners! @see Ext.ux.WindowFactory
                    scope: this,
                    'folderselect': this.onSelectFolder
                }
            });
        }
    },
    
    /**
     * select folder event listener
     * 
     * @param {Ext.tree.AsyncTreeNode} node
     */
    onSelectFolder: function(node) {
        this.selectPanel.close();
        this.setValue(node.attributes.globalname);
        this.el.focus();
    }
});
Ext.reg('felamimailfolderselect', Tine.Felamimail.FolderSelectTriggerField);
/*
 * Tine 2.0
 * 
 * @package     Felamimail
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schuele <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2010 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */
 
Ext.namespace('Tine.Felamimail');

/**
 * @namespace   Tine.Felamimail
 * @class       Tine.Felamimail.FolderSelectPanel
 * @extends     Ext.Panel
 * 
 * <p>Account/Folder Tree Panel</p>
 * <p>Tree of Accounts with folders</p>
 * <pre>
 * TODO         show error if no account(s) available
 * TODO         make it possible to preselect folder
 * TODO         use it for folder subscriptions
 * </pre>
 * 
 * @author      Philipp Schuele <p.schuele@metaways.de>
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * 
 * @param       {Object} config
 * @constructor
 * Create a new Tine.Felamimail.FolderSelectPanel
 */
Tine.Felamimail.FolderSelectPanel = Ext.extend(Ext.Panel, {
    
    /**
     * Panel config
     * @private
     */
    frame: true,
    border: true,
    autoScroll: true,
    bodyStyle: 'background-color:white',
    selectedNode: null,
    
    /**
     * init
     * @private
     */
    initComponent: function() {
        this.addEvents(
            /**
             * @event folderselect
             * Fired when folder is selected
             */
            'folderselect'
        );

        this.app = Tine.Tinebase.appMgr.get('Felamimail');
        
        if (! this.allAccounts) {
            this.account = this.account || this.app.getActiveAccount();
        }
        
        this.initActions();
        this.initFolderTree();
        
        Tine.Felamimail.FolderSelectPanel.superclass.initComponent.call(this);
    },
    
    /**
     * init actions
     */
    initActions: function() {
        this.action_cancel = new Ext.Action({
            text: _('Cancel'),
            minWidth: 70,
            scope: this,
            handler: this.onCancel,
            iconCls: 'action_cancel'
        });
        
        this.action_ok = new Ext.Action({
            disabled: true,
            text: _('Ok'),
            iconCls: 'action_saveAndClose',
            minWidth: 70,
            handler: this.onOk,
            scope: this
        });
        
        this.fbar = [
            '->',
            this.action_cancel,
            this.action_ok
        ];
    },
        
    /**
     * init folder tree
     */
    initFolderTree: function() {
        
        if (this.allAccounts) {

            this.root = new Ext.tree.TreeNode({
                text: 'default',
                draggable: false,
                allowDrop: false,
                expanded: true,
                leaf: false,
                id: 'root'
            });
        
            var mainApp = Ext.ux.PopupWindowMgr.getMainWindow().Tine.Tinebase.appMgr.get('Felamimail');
            mainApp.getAccountStore().each(function(record) {
                // TODO generalize this
                var node = new Ext.tree.AsyncTreeNode({
                    id: record.data.id,
                    path: '/' + record.data.id,
                    record: record,
                    globalname: '',
                    draggable: false,
                    allowDrop: false,
                    expanded: false,
                    text: Ext.util.format(record.get('name')),
                    qtip: Tine.Tinebase.common.doubleEncode(record.get('host')),
                    leaf: false,
                    cls: 'felamimail-node-account',
                    delimiter: record.get('delimiter'),
                    ns_personal: record.get('ns_personal'),
                    account_id: record.data.id
                });
            
                this.root.appendChild(node);
            }, this);
            
        } else {
            this.root = new Ext.tree.AsyncTreeNode({
                text: this.account.get('name'),
                draggable: false,
                allowDrop: false,
                expanded: true,
                leaf: false,
                cls: 'felamimail-node-account',
                id: this.account.id,
                path: '/' + this.account.id
            });
        }
        
        
        this.folderTree = new Ext.tree.TreePanel({
            id: 'felamimail-foldertree',
            rootVisible: ! this.allAccounts,
            store: this.store || this.app.getFolderStore(),
            // TODO use another loader/store for subscriptions
            loader: this.loader || new Tine.Felamimail.TreeLoader({
                folderStore: this.store,
                app: this.app
            }),
            root: this.root
        });
        this.folderTree.on('dblclick', this.onTreeNodeDblClick, this);
        this.folderTree.on('click', this.onTreeNodeClick, this);
        
        this.items = [this.folderTree];
    },
    
    /**
     * @private
     */
    afterRender: function() {
        Tine.Felamimail.FolderSelectPanel.superclass.afterRender.call(this);
        
        var title = (! this.allAccounts) 
            ? String.format(this.app.i18n._('Folders of account {0}'), this.account.get('name'))
            : this.app.i18n._('Folders of all accounts');
            
        this.window.setTitle(title);
    },

    /**
     * on folder select handler
     * 
     * @param {Ext.tree.AsyncTreeNode} node
     * @private
     */
    onTreeNodeDblClick: function(node) {
        this.selectedNode = node;
        this.onOk();
        return false;
    },
    
    /**
     * @private
     */
    onTreeNodeClick: function(node) {
        this.selectedNode = node;
        this.action_ok.setDisabled(false);
    },
    
    /**
     * @private
     */
    onCancel: function(){
        this.purgeListeners();
        this.window.close();
    },
    
    /**
     * @private
     */
    onOk: function() {
        if (this.selectedNode) {
            this.fireEvent('folderselect', this.selectedNode);
        }
    }
});

/**
 * Felamimail FolderSelectPanel Popup
 * 
 * @param   {Object} config
 * @return  {Ext.ux.Window}
 */
Tine.Felamimail.FolderSelectPanel.openWindow = function (config) {
    var window = Tine.WindowFactory.getWindow({
        width: 200,
        height: 300,
        modal: true,
        name: Tine.Felamimail.FolderSelectPanel.prototype.windowNamePrefix + Ext.id(),
        contentPanelConstructor: 'Tine.Felamimail.FolderSelectPanel',
        contentPanelConstructorConfig: config
    });
    return window;
};
/*
 * Tine 2.0
 * 
 * @package     Felamimail
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2010-2012 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */
 
Ext.namespace('Tine.Felamimail.sieve');

/**
 * @namespace   Tine.Felamimail
 * @class       Tine.Felamimail.sieve.VacationEditDialog
 * @extends     Tine.widgets.dialog.EditDialog
 * 
 * <p>Sieve Filter Dialog</p>
 * <p>This dialog is editing sieve filters (vacation and rules).</p>
 * <p>
 * </p>
 * 
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * 
 * @param       {Object} config
 * @constructor
 * Create a new VacationEditDialog
 */
 Tine.Felamimail.sieve.VacationEditDialog = Ext.extend(Tine.widgets.dialog.EditDialog, {

    /**
     * @cfg {Tine.Felamimail.Model.Account}
     */
    account: null,
    
    /**
     * @private
     */
    windowNamePrefix: 'VacationEditWindow_',
    appName: 'Felamimail',
    recordClass: Tine.Felamimail.Model.Vacation,
    recordProxy: Tine.Felamimail.vacationBackend,
    loadRecord: true,
    tbarItems: [],
    evalGrants: false,
    readonlyReason: false,
    
    /**
     * overwrite update toolbars function (we don't have record grants yet)
     * 
     * @private
     */
    updateToolbars: function() {
    },
    
    /**
     * executed after record got updated from proxy
     * 
     * @private
     */
    onRecordLoad: function() {
        // interrupt process flow till dialog is rendered
        if (! this.rendered) {
            this.onRecordLoad.defer(250, this);
            return;
        }
        
        // mime type is always multipart/alternative
        this.record.set('mime', 'multipart/alternative');
        if (this.account && this.account.get('signature')) {
            this.record.set('signature', this.account.get('signature'));
        }

        this.getForm().loadRecord(this.record);
        
        var title = String.format(this.app.i18n._('Vacation Message for {0}'), this.account.get('name'));
        this.window.setTitle(title);
        
        this.reasonEditor.setDisabled(! this.record.get('enabled'));
        
        Tine.log.debug('Tine.Felamimail.sieve.VacationEditDialog::onRecordLoad() -> record:');
        Tine.log.debug(this.record);
        
        this.loadMask.hide();
    },
        
    /**
     * returns dialog
     * 
     * NOTE: when this method gets called, all initalisation is done.
     * 
     * @return {Object}
     * @private
     * 
     */
    getFormItems: function() {
        
        this.initReasonEditor();
        
        var generalItems = this.getGeneralItems();
        
        return {
            xtype: 'tabpanel',
            deferredRender: false,
            border: false,
            activeTab: 0,
            items: [{
                title: this.app.i18n._('General'),
                autoScroll: true,
                border: false,
                frame: true,
                xtype: 'columnform',
                formDefaults: {
                    anchor: '100%',
                    labelSeparator: '',
                    columnWidth: 1
                },
                items: generalItems
            }, {
                title: this.app.i18n._('Advanced'),
                autoScroll: true,
                border: false,
                frame: true,
                xtype: 'columnform',
                formDefaults: {
                    anchor: '100%',
                    labelSeparator: '',
                    columnWidth: 1
                },
                items: [[{
                    fieldLabel: this.app.i18n._('Only send all X days to the same sender'),
                    name: 'days',
                    value: 7,
                    xtype: 'numberfield',
                    allowNegative: false,
                    minValue: 1
                }]]
            }]
        };
    },
    
    /**
     * init reason editor
     */
    initReasonEditor: function() {
        var reg = this.app.getRegistry(),
            readonly = reg.get('config').vacationMessageCustomAllowed && reg.get('config').vacationMessageCustomAllowed.value === 0;
        
        this.reasonEditor = new Ext.form.HtmlEditor({
            fieldLabel: this.app.i18n._('Incoming mails will be answered with this text:'),
            name: 'reason',
            allowBlank: true,
            disabled: true,
            height: 220,
            readOnly: readonly,
            getDocMarkup: function() {
                var markup = '<html><body></body></html>';
                return markup;
            },
            plugins: [
                new Ext.ux.form.HtmlEditor.RemoveFormat()
            ]
        });
    },
    
    /**
     * get items for general tab
     * 
     * @return Array
     */
    getGeneralItems: function() {
        var items = [[{
            fieldLabel: this.app.i18n._('Status'),
            name: 'enabled',
            typeAhead     : false,
            triggerAction : 'all',
            lazyRender    : true,
            editable      : false,
            mode          : 'local',
            forceSelection: true,
            value: 0,
            xtype: 'combo',
            store: [
                [0, this.app.i18n._('I am available (vacation message disabled)')], 
                [1, this.app.i18n._('I am not available (vacation message enabled)')]
            ],
            listeners: {
                scope: this,
                select: function (combo, record) {
                    this.reasonEditor.setDisabled(! record.data.field1);
                }
            }
        }]];
        
        // add vacation template items if needed
        var templates = this.app.getRegistry().get('vacationTemplates');
        if (templates.totalcount > 0) {
            items = items.concat(this.getTemplateItems(templates));
        }
        
        items.push([this.reasonEditor]);
        
        return items;
    },
    
    /**
     * get items for vacation templates
     * 
     * @param Object templates
     * @return Array
     * 
     * TODO use grid panel for x representatives?
     */
    getTemplateItems: function(templates) {
        Tine.log.debug('Tine.Felamimail.sieve.VacationEditDialog::getTemplateItems()');
        Tine.log.debug(templates);
        
        var commonConfig = {
                listeners: {
                    scope: this,
                    select: this.onSelectTemplateField
                },
                columnWidth: 0.5,
            },
            items = [[Ext.apply({
            fieldLabel: this.app.i18n._('Start Date'),
            emptyText: this.app.i18n._('Set vacation start date ...'),
            name: 'start_date',
            xtype: 'datefield'
        }, commonConfig), Ext.apply({
            fieldLabel: this.app.i18n._('End Date'),
            emptyText: this.app.i18n._('Set vacation end date ...'),
            name: 'end_date',
            xtype: 'datefield'
        }, commonConfig)], [
            new Tine.Addressbook.SearchCombo(Ext.apply({
                fieldLabel: this.app.i18n._('Representative #1'),
                emptyText: this.app.i18n._('Choose first Representative ...'),
                blurOnSelect: true,
                name: 'contact_id1',
                selectOnFocus: true,
                forceSelection: false,
                allowBlank: true
            }, commonConfig)),
            new Tine.Addressbook.SearchCombo(Ext.apply({
                fieldLabel: this.app.i18n._('Representative #2'),
                emptyText: this.app.i18n._('Choose second Representative ...'),
                blurOnSelect: true,
                name: 'contact_id2',
                selectOnFocus: true,
                forceSelection: false,
                allowBlank: true
            }, commonConfig))
        ], [{
            fieldLabel: this.app.i18n._('Message Template'),
            xtype: 'combo',
            mode: 'local',
            listeners: {
                scope: this,
                select: this.onTemplateComboSelect
            },
            displayField: 'name',
            name: 'template_id',
            valueField: 'id',
            triggerAction: 'all',
            emptyText: this.app.i18n._('Choose Template ...'),
            editable: false,
            store: new Ext.data.JsonStore({
                id: 'timezone',
                root: 'results',
                totalProperty: 'totalcount',
                fields: ['id', 'name', 'type'], // TODO use Tine.Filemanager.Model.Node or generic File model?
                data: templates
            })
        }]
        ];
        
        return items;
    },
    
    /**
     * template field has been selected, check if new vacation message needs to be fetched
     * - do this only if template has already been selected
     */
    onSelectTemplateField: function() {
        if (this.record.get('template_id') !== '') {
            this.getVacationMessage();
        }
    },
    
    /**
     * template combo select event handler
     * 
     * @param {} combo
     * @param {} record
     * @param {} index
     */
    onTemplateComboSelect: function(combo, record, index) {
        Tine.log.debug('Tine.Felamimail.sieve.VacationEditDialog::onTemplateComboSelect()');
        Tine.log.debug(record);
        
        if (record.data && record.get('type') === 'file') {
            this.getVacationMessage();
        } else {
            // TODO do something?
        }
    },
    
    /**
     * get vacation with template replacements message from server
     */
    getVacationMessage: function() {
        this.loadMask.show();
        this.onRecordUpdate();
        Tine.Felamimail.getVacationMessage(this.record.data, this.onGetVacationMessage.createDelegate(this));
    },
    
    /**
     * onGetVacationMessage
     * 
     * @param {} response
     */
    onGetVacationMessage: function(response) {
        Tine.log.debug('Tine.Felamimail.sieve.VacationEditDialog::onGetMessage()');
        Tine.log.debug(response);
        this.loadMask.hide();
        
        if (response.message) {
            this.reasonEditor.setValue(response.message);
        }
    },
    
    /**
     * generic request exception handler
     * 
     * @param {Object} exception
     */
    onRequestFailed: function(exception) {
        Tine.Felamimail.handleRequestException(exception);
        this.loadMask.hide();
    },
    
    /**
     * executed when record gets updated from form
     */
    onRecordUpdate: function() {
        Tine.Felamimail.sieve.VacationEditDialog.superclass.onRecordUpdate.call(this);
        
        var contactIds = [];
        Ext.each(['contact_id1', 'contact_id2'], function(field) {
            if (this.getForm().findField(field) && this.getForm().findField(field).getValue() !== '') {
                contactIds.push(this.getForm().findField(field).getValue());
            }
        }, this);
        this.record.set('contact_ids', contactIds);
    }
});

/**
 * Felamimail Edit Popup
 * 
 * @param   {Object} config
 * @return  {Ext.ux.Window}
 */
Tine.Felamimail.sieve.VacationEditDialog.openWindow = function (config) {
    var window = Tine.WindowFactory.getWindow({
        width: 640,
        height: 550,
        name: Tine.Felamimail.sieve.VacationEditDialog.prototype.windowNamePrefix + Ext.id(),
        contentPanelConstructor: 'Tine.Felamimail.sieve.VacationEditDialog',
        contentPanelConstructorConfig: config
    });
    return window;
};
/*
 * Tine 2.0
 * 
 * @package     Felamimail
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2010-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */
 
Ext.namespace('Tine.Felamimail.sieve');

/**
 * @namespace   Tine.Felamimail.sieve
 * @class       Tine.Felamimail.sieve.RuleEditDialog
 * @extends     Tine.widgets.dialog.EditDialog
 * 
 * <p>Sieve Filter Dialog</p>
 * <p>This dialog is editing a filter rule.</p>
 * <p>
 * </p>
 * 
 * @author      Philipp Schuele <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2010 Metaways Infosystems GmbH (http://www.metaways.de)
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * 
 * @param       {Object} config
 * @constructor
 * Create a new RuleEditDialog
 */
Tine.Felamimail.sieve.RuleEditDialog = Ext.extend(Tine.widgets.dialog.EditDialog, {
    /**
     * @cfg {Tine.Felamimail.Model.Account}
     */
    account: null,
    
    /**
     * @private
     */
    windowNamePrefix: 'RuleEditWindow_',
    appName: 'Felamimail',
    recordClass: Tine.Felamimail.Model.Rule,
    mode: 'local',
    loadRecord: true,
    tbarItems: [],
    evalGrants: false,
    
    /**
     * overwrite update toolbars function (we don't have record grants yet)
     * 
     * @private
     */
    updateToolbars: function() {

    },
    
    /**
     * @private
     */
    onRender: function(ct, position) {
        Tine.Felamimail.sieve.RuleEditDialog.superclass.onRender.call(this, ct, position);
        
        this.onChangeType.defer(250, this);
    },
    
    /**
     * Change type card layout depending on selected combo box entry and set field value
     */
    onChangeType: function() {
        var type = this.actionTypeCombo.getValue();
        
        var cardLayout = Ext.getCmp(this.idPrefix + 'CardLayout').getLayout();
        if (cardLayout !== 'card') {
            cardLayout.setActiveItem(this.idPrefix + type);
            if (this.record.get('action_type') == type) {
                var field = this.getForm().findField('action_argument_' + type);
                if (field !== null) {
                    field.setValue(this.record.get('action_argument'));
                }
            }
        }
    },
    
    /**
     * executed after record got updated from proxy
     * 
     * @private
     */
    onRecordLoad: function() {
        // interrupt process flow till dialog is rendered
        if (! this.rendered) {
            this.onRecordLoad.defer(250, this);
            return;
        }
        
        var title = this.app.i18n._('Edit Filter Rule');
        this.window.setTitle(title);
        
        this.getForm().loadRecord(this.record);
        
        this.loadMask.hide();
    },
    
    /**
     * @private
     */
    onRecordUpdate: function() {
        Tine.Felamimail.sieve.RuleEditDialog.superclass.onRecordUpdate.call(this);
        
        this.record.set('conditions', this.getConditions());
        
        var argumentField = this.getForm().findField('action_argument_' + this.actionTypeCombo.getValue()),
            argumentValue = (argumentField !== null) ? argumentField.getValue() : '';
        this.record.set('action_argument', argumentValue);
    },
    
    /**
     * get conditions and do the mapping
     * 
     * @return {Array}
     */
    getConditions: function() {
        var conditions = this.conditionsPanel.getAllFilterData();
        var result = [],
            i = 0, 
            condition,
            test,
            comperator,
            header;
            
        for (i = 0; i < conditions.length; i++) {
            // set defaults
            comperator = conditions[i].operator;
            header = conditions[i].field;
            test = 'header';

            switch (conditions[i].field) {
                case 'from':
                case 'to':
                    test = 'address';
                    break;
                case 'fromheader':
                    header = 'From';
                    break;
                case 'size':
                    test = 'size';
                    comperator = (conditions[i].operator == 'greater') ? 'over' : 'under';
                    break;
                case 'header':
                    header = conditions[i].operator;
                    comperator = 'contains';
                    break;
                case 'headerregex':
                    header = conditions[i].operator;
                    comperator = 'regex';
                    break;
            }
            condition = {
                test: test,
                header: header,
                comperator: comperator,
                key: conditions[i].value
            };
            result.push(condition);
        }
        return result;
    },
    
    /**
     * get conditions filter data (reverse of getConditions)
     * 
     * @return {Array}
     */
    getConditionsFilter: function() {
        var conditions = this.record.get('conditions');
        var result = [],
            i = 0, 
            filter,
            operator,
            field;
            
        for (i = 0; i < conditions.length; i++) {
            field = conditions[i].header;
            switch (field) {
                case 'size':
                    operator = (conditions[i].comperator == 'over') ? 'greater' : 'less';
                    break;
                case 'from':
                case 'to':
                case 'subject':
                    operator = conditions[i].comperator;
                    break;
                default:
                    if (field == 'From') {
                        operator = conditions[i].comperator;
                        field = 'fromheader';
                    } else {
                        operator = field;
                        field = (conditions[i].comperator == 'contains') ? 'header' : 'headerregex';
                    }
            }
            filter = {
                field: field,
                operator: operator,
                value: conditions[i].key
            };
            result.push(filter);
        }
        
        return result;
    },
    
    /**
     * returns dialog
     * 
     * NOTE: when this method gets called, all initalisation is done.
     * 
     * @return {Object}
     * @private
     */
    getFormItems: function() {
        
        this.conditionsPanel = new Tine.Felamimail.sieve.RuleConditionsPanel({
            filters: this.getConditionsFilter()
        });
        
        this.actionTypeCombo = new Ext.form.ComboBox({
            hideLabel       : true,
            name            : 'action_type',
            typeAhead       : false,
            triggerAction   : 'all',
            lazyRender      : true,
            editable        : false,
            mode            : 'local',
            forceSelection  : true,
            value           : 'discard',
            columnWidth     : 0.4,
            store: Tine.Felamimail.sieve.RuleEditDialog.getActionTypes(this.app),
            listeners: {
                scope: this,
                change: this.onChangeType,
                select: this.onChangeType
            }
        });
        
        this.idPrefix = Ext.id();
        
        return [{
            xtype: 'panel',
            layout: 'border',
            autoScroll: true,
            items: [
            {
                title: this.app.i18n._('If all of the following conditions are met:'),
                region: 'north',
                border: false,
                autoScroll: true,
                items: [
                    this.conditionsPanel
                ],
                xtype: 'panel',
                listeners: {
                    scope: this,
                    afterlayout: function(ct, layout) {
                        ct.suspendEvents();
                        if (this.conditionsPanel.getHeight() < 170) {
                            ct.setHeight(this.conditionsPanel.getHeight() + 30);
                        }
                        ct.ownerCt.layout.layout();
                        ct.resumeEvents();
                    }
                }
            }, {
                title: this.app.i18n._('Do this action:'),
                region: 'center',
                border: false,
                frame: true,
                layout: 'column',
                items: [
                    this.actionTypeCombo,
                    // TODO try to add a spacer/margin between the two input fields
                /*{
                    // spacer
                    columnWidth: 0.1,
                    layout: 'fit',
                    title: '',
                    items: []
                }, */{
                    id: this.idPrefix + 'CardLayout',
                    layout: 'card',
                    activeItem: this.idPrefix + 'fileinto',
                    border: false,
                    columnWidth: 0.5,
                    defaults: {
                        border: false
                    },
                    items: [{
                        id: this.idPrefix + 'fileinto',
                        layout: 'form',
                        items: [{
                            name: 'action_argument_fileinto',
                            xtype: 'felamimailfolderselect',
                            width: 200,
                            hideLabel: true,
                            account: this.account
                        }]
                    }, {
                        // TODO add email validator?
                        id: this.idPrefix + 'redirect',
                        layout: 'form',
                        items: [{
                            name: 'action_argument_redirect',
                            xtype: 'textfield',
                            emptyText: 'test@example.org',
                            width: 200,
                            hideLabel: true
                        }]
                    }, {
                        id: this.idPrefix + 'reject',
                        layout: 'form',
                        items: [{
                            name: 'action_argument_reject',
                            xtype: 'textarea',
                            width: 300,
                            height: 60,
                            hideLabel: true
                        }]
                    }, {
                        id: this.idPrefix + 'discard',
                        layout: 'fit',
                        items: []
                    }]
                }]
            }]
        }];
    }
});

/**
 * Felamimail Edit Popup
 * 
 * @param   {Object} config
 * @return  {Ext.ux.Window}
 */
Tine.Felamimail.sieve.RuleEditDialog.openWindow = function (config) {
    var window = Tine.WindowFactory.getWindow({
        width: 700,
        height: 300,
        name: Tine.Felamimail.sieve.RuleEditDialog.prototype.windowNamePrefix + Ext.id(),
        contentPanelConstructor: 'Tine.Felamimail.sieve.RuleEditDialog',
        contentPanelConstructorConfig: config
    });
    return window;
};

/**
 * get action types for action combo and action type renderer
 * 
 * @param {} app
 * @return {Array}
 */
Tine.Felamimail.sieve.RuleEditDialog.getActionTypes = function(app) {
    return [
        ['fileinto',    app.i18n._('Move mail to folder')],
        ['redirect',    app.i18n._('Redirect mail to address')],
        ['reject',      app.i18n._('Reject mail with this text')],
        ['discard',     app.i18n._('Discard mail')]
        //['keep',        app.i18n._('Keep mail')],
    ];
};
/*
 * Tine 2.0
 * 
 * @package     Felamimail
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2010-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */
 
Ext.ns('Tine.Felamimail.sieve');

/**
 * @namespace Tine.Felamimail
 * @class     Tine.Felamimail.sieve.RulesGridPanel
 * @extends   Tine.widgets.grid.GridPanel
 * Rules Grid Panel <br>
 * 
 * @author      Philipp Schuele <p.schuele@metaways.de>
 */
Tine.Felamimail.sieve.RulesGridPanel = Ext.extend(Tine.widgets.grid.GridPanel, {
    /**
     * @cfg {Tine.Felamimail.Model.Account}
     */
    account: null,
    
    // model generics
    recordClass: Tine.Felamimail.Model.Rule,
    recordProxy: Tine.Felamimail.rulesBackend,
    
    // grid specific
    defaultSortInfo: {field: 'id', dir: 'ASC'},
    storeRemoteSort: false,
    
    // not yet
    evalGrants: false,
    usePagingToolbar: false,
    splitAddButton: false,
    
    newRecordIcon: 'action_new_rule',
    editDialogClass: Tine.Felamimail.sieve.RuleEditDialog,
    
    initComponent: function() {
        this.app = Tine.Tinebase.appMgr.get('Felamimail');
        this.initColumns();
        
        this.editDialogConfig = {
            account: this.account
        };
        
        this.supr().initComponent.call(this);
    },
    
    /**
     * Return CSS class to apply to rows depending on enabled status
     * 
     * @param {Tine.Felamimail.Model.Rule} record
     * @param {Integer} index
     * @return {String}
     */
    getViewRowClass: function(record, index) {
        var className = '';
        
        if (! record.get('enabled')) {
            className += ' felamimail-sieverule-disabled';
        }
        
        return className;
    },
    
    /**
     * init actions with actionToolbar, contextMenu and actionUpdater
     * 
     * @private
     */
    initActions: function() {
        this.action_moveup = new Ext.Action({
            text: this.app.i18n._('Move up'),
            handler: this.onMoveRecord.createDelegate(this, ['up']),
            scope: this,
            iconCls: 'action_move_up'
        });

        this.action_movedown = new Ext.Action({
            text: this.app.i18n._('Move down'),
            handler: this.onMoveRecord.createDelegate(this, ['down']),
            scope: this,
            iconCls: 'action_move_down'
        });

        this.action_enable = new Ext.Action({
            text: this.app.i18n._('Enable'),
            handler: this.onEnableDisable.createDelegate(this, [true]),
            scope: this,
            iconCls: 'action_enable'
        });

        this.action_disable = new Ext.Action({
            text: this.app.i18n._('Disable'),
            handler: this.onEnableDisable.createDelegate(this, [false]),
            scope: this,
            iconCls: 'action_disable'
        });
        
        this.supr().initActions.call(this);
    },
    
    /**
     * enable / disable rule
     * 
     * @param {Boolean} state
     */
    onEnableDisable: function(state) {
        var selectedRows = this.grid.getSelectionModel().getSelections();
        for (var i = 0; i < selectedRows.length; i++) {
            selectedRows[i].set('enabled', state);
        }
    },
    
    /**
     * move record up or down
     * 
     * @param {String} dir (up|down)
     */
    onMoveRecord: function(dir) {
        var sm = this.grid.getSelectionModel();
            
        if (sm.getCount() == 1) {
            var selectedRows = sm.getSelections();
            record = selectedRows[0];
            
            // get next/prev record
            var index = this.store.indexOf(record),
                switchRecordIndex = (dir == 'down') ? index + 1 : index - 1,
                switchRecord = this.store.getAt(switchRecordIndex);
            
            if (switchRecord) {
                // switch ids and resort store
                var oldId = record.id;
                    switchId = switchRecord.id;

                record.set('id', Ext.id());
                record.id = Ext.id();
                switchRecord.set('id', oldId);
                switchRecord.id = oldId;
                record.set('id', switchId);
                record.id = switchId;
                
                this.store.commitChanges();
                this.store.sort('id', 'ASC');
                sm.selectRecords([record]);
            }
        }
    },

    /**
     * add custom items to action toolbar
     * 
     * @return {Object}
     */
    getActionToolbarItems: function() {
        return [
            {
                xtype: 'buttongroup',
                columns: 1,
                frame: false,
                items: [
                    this.action_moveup,
                    this.action_movedown
                ]
            }
        ];
    },
    
    /**
     * add custom items to context menu
     * 
     * @return {Array}
     */
    getContextMenuItems: function() {
        var items = [
            '-',
            this.action_moveup,
            this.action_movedown,
            '-',
            this.action_enable,
            this.action_disable
        ];
        
        return items;
    },
    
    /**
     * init columns
     */
    initColumns: function() {
        this.gridConfig.columns = [
        {
            id: 'id',
            header: this.app.i18n._("ID"),
            width: 40,
            sortable: false,
            dataIndex: 'id',
            hidden: true
        }, {
            id: 'conditions',
            header: this.app.i18n._("Conditions"),
            width: 200,
            sortable: false,
            dataIndex: 'conditions',
            scope: this,
            renderer: this.conditionsRenderer
        }, {
            id: 'action',
            header: this.app.i18n._("Action"),
            width: 250,
            sortable: false,
            dataIndex: 'action_type',
            scope: this,
            renderer: this.actionRenderer
        }];
    },
    
    /**
     * init layout
     */
    initLayout: function() {
        this.supr().initLayout.call(this);
        
        this.items.push({
            region : 'north',
            height : 55,
            border : false,
            items  : this.actionToolbar
        });
    },
    
    /**
     * preform the initial load of grid data
     */
    initialLoad: function() {
        this.store.load.defer(10, this.store, [
            typeof this.autoLoad == 'object' ?
                this.autoLoad : undefined]);
    },
    
    /**
     * called before store queries for data
     */
    onStoreBeforeload: function(store, options) {
        Tine.Felamimail.sieve.RulesGridPanel.superclass.onStoreBeforeload.call(this, store, options);
        
        options.params.filter = this.account.id;
    },
    
    /**
     * action renderer
     * 
     * @param {Object} type
     * @param {Object} metadata
     * @param {Object} record
     * @return {String}
     */
    actionRenderer: function(type, metadata, record) {
        var types = Tine.Felamimail.sieve.RuleEditDialog.getActionTypes(this.app),
            result = type;
        
        for (i=0; i < types.length; i++) {
            if (types[i][0] == type) {
                result = types[i][1];
            }
        }
        
        if (record.get('action_argument') && record.get('action_argument') != '') {
            result += ' ' + record.get('action_argument');
        }
            
        return Ext.util.Format.htmlEncode(result);
    },

    /**
     * conditions renderer
     * 
     * @param {Object} value
     * @return {String}
     * 
     * TODO show more conditions?
     */
    conditionsRenderer: function(value) {
        var result = '';
        
        // show only first condition
        if (value && value.length > 0) {
            var condition = value[0];
            
            // get header/comperator translation
            var filterModels = Tine.Felamimail.sieve.RuleConditionsPanel.getFilterModel(this.app),
                header, 
                comperator, 
                found = false;
            Ext.each(filterModels, function(filterModel) {
                if (condition.header == filterModel.field) {
                    header = filterModel.label;
                    if (condition.header == 'size') {
                        comperator = (condition.comperator == 'over') ? _('is greater than') : _('is less than');
                    } else {
                        comperator = _(condition.comperator);
                    }
                    found = true;
                }
            }, this);
            
            if (found === true) {
                result = header + ' ' + comperator + ' "' + condition.key + '"';
            } else {
                result = (condition.comperator == 'contains') ? this.app.i18n._('Header "{0}" contains "{1}"') : this.app.i18n._('Header "{0}" matches "{1}"');
                result = String.format(result, condition.header, condition.key);
            }
        }
        
        return Ext.util.Format.htmlEncode(result);
    },
    
    /**
     * on update after edit
     * 
     * @param {String} encodedRecordData (json encoded)
     */
    onUpdateRecord: function(encodedRecordData) {
        var newRecord = Tine.Felamimail.rulesBackend.recordReader({responseText: encodedRecordData});
        
        if (! newRecord.id) {
            var lastRecord = null,
                nextId = null;
            do {
                // get next free id
                lastRecord = this.store.getAt(this.store.getCount()-1);
                nextId = (lastRecord) ? (parseInt(lastRecord.id, 10) + 1).toString() : '1';
            } while (this.store.getById(newRecord.id));
            
            newRecord.set('id', nextId);
            newRecord.id = nextId;
        } else {
            this.store.remove(this.store.getById(newRecord.id));
        }
        
        this.store.addSorted(newRecord);
        
        // some eyecandy
        var row = this.getView().getRow(this.store.indexOf(newRecord));
        Ext.fly(row).highlight();
    },
    
    /**
     * generic delete handler
     */
    onDeleteRecords: function(btn, e) {
        var sm = this.grid.getSelectionModel();
        var records = sm.getSelections();
        Ext.each(records, function(record) {
            this.store.remove(record);
        });
    }
});
/*
 * Tine 2.0
 * 
 * @package     Felamimail
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2010-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */
 
Ext.namespace('Tine.Felamimail.sieve');

/**
 * @namespace   Tine.Felamimail
 * @class       Tine.Felamimail.sieve.RulesDialog
 * @extends     Tine.widgets.dialog.EditDialog
 * 
 * <p>Sieve Filter Dialog</p>
 * <p>This dialog is for editing sieve filters (rules).</p>
 * <p>
 * </p>
 * 
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * 
 * @param       {Object} config
 * @constructor
 * Create a new RulesDialog
 */
Tine.Felamimail.sieve.RulesDialog = Ext.extend(Tine.widgets.dialog.EditDialog, {

    /**
     * @cfg {Tine.Felamimail.Model.Account}
     */
    account: null,

    /**
     * @private
     */
    windowNamePrefix: 'VacationEditWindow_',
    appName: 'Felamimail',
//    loadRecord: false,
    mode: 'local',
    tbarItems: [],
    evalGrants: false,
    
    //private
    initComponent: function(){
        Tine.Felamimail.sieve.RulesDialog.superclass.initComponent.call(this);
        
        this.i18nRecordName = this.app.i18n._('Sieve Filter Rules');
    },
    
    /**
     * overwrite update toolbars function (we don't have record grants yet)
     * 
     * @private
     */
    updateToolbars: Ext.emptyFn,
    
    /**
     * init record to edit
     * -> we don't have a real record here
     */
    initRecord: function() {
        this.onRecordLoad();
    },
    
    /**
     * executed after record got updated from proxy
     * -> we don't have a real record here
     * 
     * @private
     */
    onRecordLoad: function() {
        // interrupt process flow till dialog is rendered
        if (! this.rendered) {
            this.onRecordLoad.defer(250, this);
            return;
        }
        
        var title = String.format(this.app.i18n._('Sieve Filter Rules for {0}'), this.account.get('name'));
        this.window.setTitle(title);
        
        this.loadMask.hide();
    },
        
    /**
     * returns dialog
     * 
     * NOTE: when this method gets called, all initalisation is done.
     * 
     * @return {Object}
     * @private
     * 
     */
    getFormItems: function() {
        this.rulesGrid = new Tine.Felamimail.sieve.RulesGridPanel({
            account: this.account
        });
        
        return [this.rulesGrid];
    },
    
    /**
     * apply changes handler (get rules and send them to saveRules)
     */
    onApplyChanges: function(closeWindow) {
        var rules = [];
        this.rulesGrid.store.each(function(record) {
            rules.push(record.data);
        });
        
        this.loadMask.show();
        Tine.Felamimail.rulesBackend.saveRules(this.account.id, rules, {
            scope: this,
            success: function(record) {
                if (closeWindow) {
                    this.purgeListeners();
                    this.window.close();
                }
            },
            failure: Tine.Felamimail.handleRequestException.createSequence(function() {
                this.loadMask.hide();
            }, this),
            timeout: 150000 // 3 minutes
        });
    }
});

/**
 * Felamimail Edit Popup
 * 
 * @param   {Object} config
 * @return  {Ext.ux.Window}
 */
Tine.Felamimail.sieve.RulesDialog.openWindow = function (config) {
    var window = Tine.WindowFactory.getWindow({
        width: 800,
        height: 400,
        name: Tine.Felamimail.sieve.RulesDialog.prototype.windowNamePrefix + Ext.id(),
        contentPanelConstructor: 'Tine.Felamimail.sieve.RulesDialog',
        contentPanelConstructorConfig: config
    });
    return window;
};
/*
 * Tine 2.0
 * 
 * @package     Felamimail
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2010-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */
 
Ext.namespace('Tine.Felamimail.sieve');

/**
 * @namespace   Tine.Felamimail.sieve
 * @class       Tine.Felamimail.sieve.RuleConditionsPanel
 * @extends     Tine.widgets.grid.FilterToolbar
 * 
 * <p>Sieve Filter Conditions Panel</p>
 * <p>
 * mapping when getting filter values:
 *  field       -> test_header or 'size'
 *  operator    -> comperator
 *  value       -> key
 * </p>
 * <p>
 * </p>
 * 
 * @param       {Object} config
 * @constructor
 * Create a new RuleConditionsPanel
 */
Tine.Felamimail.sieve.RuleConditionsPanel = Ext.extend(Tine.widgets.grid.FilterToolbar, {
    
    defaultFilter: 'from',
    neverAllowSaving: true,
    showSearchButton: false,
    filterFieldWidth: 160,
    
    // unused fn
    onFiltertrigger: Ext.emptyFn,
    
    initComponent: function() {
        this.app = Tine.Tinebase.appMgr.get('Felamimail');
        this.rowPrefix = '';
        
        this.filterModels = Tine.Felamimail.sieve.RuleConditionsPanel.getFilterModel(this.app);
        
        this.supr().initComponent.call(this);
    },
    
    /**
     * gets filter data (use getValue() if we don't have a store/plugins)
     * 
     * @return {Array} of filter records
     */
    getAllFilterData: function() {
        return this.getValue();
    }
});

/**
 * get rule conditions for filter model and condition renderer
 * 
 * @param {} app
 * @return {Array}
 */
Tine.Felamimail.sieve.RuleConditionsPanel.getFilterModel = function(app) {
    return [
        {label: app.i18n._('From (Email)'),     field: 'from',     operators: ['contains', 'regex'], emptyText: 'test@example.org'},
        {label: app.i18n._('From (Email and Name)'), field: 'fromheader',     operators: ['contains', 'regex'], emptyText: 'name or email'},
        {label: app.i18n._('To (Email)'),       field: 'to',       operators: ['contains', 'regex'], emptyText: 'test@example.org'},
        {label: app.i18n._('Subject'),          field: 'subject',  operators: ['contains', 'regex'], emptyText: app.i18n._('Subject')},
        {label: app.i18n._('Size'),             field: 'size',     operators: ['greater', 'less'], valueType: 'number', defaultOperator: 'greater'},
        {label: app.i18n._('Header contains'),  field: 'header',   operators: ['freeform'], defaultOperator: 'freeform', 
            emptyTextOperator: app.i18n._('Header name'), emptyText: app.i18n._('Header value')},
        {label: app.i18n._('Header regex'),     field: 'headerregex',   operators: ['freeform'], defaultOperator: 'freeform',
            emptyTextOperator: app.i18n._('Header name'), emptyText: app.i18n._('Header value')}
    ];
};
/*
 * Tine 2.0
 * 
 * @package     Felamimail
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2010-2012 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */
 
Ext.namespace('Tine.Felamimail');

/**
 * get felamimail tree panel context menus
 * this is used in Tine.Felamimail.TreePanel (with createDelegate)
 * 
 * TODO use Ext.apply to get this
 */
Tine.Felamimail.setTreeContextMenus = function() {
    
    // define additional actions
    var emptyFolderAction = {
        text: this.app.i18n._('Empty Folder'),
        iconCls: 'action_folder_emptytrash',
        scope: this,
        handler: function() {
            this.ctxNode.getUI().addClass("x-tree-node-loading");
            var folderId = this.ctxNode.attributes.folder_id;
            Ext.Ajax.request({
                params: {
                    method: 'Felamimail.emptyFolder',
                    folderId: folderId
                },
                scope: this,
                success: function(result, request){
                    var selectedNode = this.getSelectionModel().getSelectedNode(),
                        isSelectedNode = (selectedNode && this.ctxNode.id == selectedNode.id);
                        
                    if (isSelectedNode) {
                        var folder = Tine.Felamimail.folderBackend.recordReader(result);
                        this.app.getFolderStore().updateFolder(folder);
                    } else {
                        var folder = this.app.getFolderStore().getById(folderId);
                        folder.set('cache_unreadcount', 0);
                    }
                    this.ctxNode.getUI().removeClass("x-tree-node-loading");
                    this.ctxNode.removeAll();
                },
                failure: function() {
                    this.ctxNode.getUI().removeClass("x-tree-node-loading");
                },
                timeout: 120000 // 2 minutes
            });
        }
    };
    
    // we need this for adding folders to account (root level)
    var addFolderToRootAction = {
        text: this.app.i18n._('Add Folder'),
        iconCls: 'action_add',
        scope: this,
        disabled: true,
        handler: function() {
            Ext.MessageBox.prompt(String.format(_('New {0}'), this.app.i18n._('Folder')), String.format(_('Please enter the name of the new {0}:'), this.app.i18n._('Folder')), function(_btn, _text) {
                if( this.ctxNode && _btn == 'ok') {
                    if (! _text) {
                        Ext.Msg.alert(String.format(_('No {0} added'), this.app.i18n._('Folder')), String.format(_('You have to supply a {0} name!'), this.app.i18n._('Folder')));
                        return;
                    }
                    Ext.MessageBox.wait(_('Please wait'), String.format(_('Creating {0}...' ), this.app.i18n._('Folder')));
                    var parentNode = this.ctxNode;
                    
                    var params = {
                        method: 'Felamimail.addFolder',
                        name: _text
                    };
                    
                    params.parent = '';
                    params.accountId = parentNode.id;
                    
                    Ext.Ajax.request({
                        params: params,
                        scope: this,
                        timeout: 150000, // 2 minutes
                        success: function(_result, _request){
                            var nodeData = Ext.util.JSON.decode(_result.responseText);
                            var newNode = this.loader.createNode(nodeData);
                            parentNode.appendChild(newNode);
                            this.fireEvent('containeradd', nodeData);
                            Ext.MessageBox.hide();
                        }
                    });
                    
                }
            }, this);
        }
    };
    
    var editAccountAction = {
        text: this.app.i18n._('Edit Account'),
        iconCls: 'FelamimailIconCls',
        scope: this,
        disabled: ! Tine.Tinebase.common.hasRight('manage_accounts', 'Felamimail'),
        handler: function() {
            var record = this.accountStore.getById(this.ctxNode.attributes.account_id);
            var popupWindow = Tine.Felamimail.AccountEditDialog.openWindow({
                record: record,
                listeners: {
                    scope: this,
                    'update': function(record) {
                        var account = new Tine.Felamimail.Model.Account(Ext.util.JSON.decode(record));
                        
                        // update tree node + store
                        this.ctxNode.setText(account.get('name'));
                        this.accountStore.reload();
                        
                        // reload tree node + remove all folders of this account from store ?
                        this.folderStore.resetQueryAndRemoveRecords('parent_path', '/' + this.ctxNode.attributes.account_id);
                        this.ctxNode.reload(function(callback) {
                        });
                    }
                }
            });
        }
    };

    var editVacationAction = {
        text: this.app.i18n._('Edit Vacation Message'),
        iconCls: 'action_email_replyAll',
        scope: this,
        handler: function() {
            var accountId = this.ctxNode.attributes.account_id;
            var account = this.accountStore.getById(accountId);
            var record = new Tine.Felamimail.Model.Vacation({id: accountId}, accountId);
            
            var popupWindow = Tine.Felamimail.sieve.VacationEditDialog.openWindow({
                account: account,
                record: record
            });
        }
    };
    
    var editRulesAction = {
        text: this.app.i18n._('Edit Filter Rules'),
        iconCls: 'action_email_forward',
        scope: this,
        handler: function() {
            var accountId = this.ctxNode.attributes.account_id;
            var account = this.accountStore.getById(accountId);
            
            var popupWindow = Tine.Felamimail.sieve.RulesDialog.openWindow({
                account: account
            });
        }
    };

    var markFolderSeenAction = {
        text: this.app.i18n._('Mark Folder as read'),
        iconCls: 'action_mark_read',
        scope: this,
        handler: function() {
            if (this.ctxNode) {
                var folderId = this.ctxNode.id,
                    filter = [{
                        field: 'folder_id',
                        operator: 'equals',
                        value: folderId
                    }, {
                        field: 'flags',
                        operator: 'notin',
                        value: ['\\Seen']
                    }
                ];
                
                var selectedNode = this.getSelectionModel().getSelectedNode(),
                    isSelectedNode = (selectedNode && this.ctxNode.id == selectedNode.id);
                
                Tine.Felamimail.messageBackend.addFlags(filter, '\\Seen', {
                    callback: function() {
                        this.app = Tine.Tinebase.appMgr.get('Felamimail');
                        var folder = this.app.getFolderStore().getById(folderId);
                        folder.set('cache_unreadcount', 0);
                        if (isSelectedNode) {
                            this.app.getMainScreen().getCenterPanel().loadGridData({
                                removeStrategy: 'keepBuffered'
                            });
                        }
                    }
                });
            }
        }
    };

    var updateFolderCacheAction = {
        text: this.app.i18n._('Update Folder List'),
        iconCls: 'action_update_cache',
        scope: this,
        handler: function() {
            if (this.ctxNode) {
                this.getSelectionModel().clearSelections();
                
                var folder = this.app.getFolderStore().getById(this.ctxNode.id),
                    account = folder ? this.app.getAccountStore().getById(folder.get('account_id')) :
                                       this.app.getAccountStore().getById(this.ctxNode.id);
                this.ctxNode.getUI().addClass("x-tree-node-loading");
                // call update folder cache
                Ext.Ajax.request({
                    params: {
                        method: 'Felamimail.updateFolderCache',
                        accountId: account.id,
                        folderName: folder ? folder.get('globalname') : ''
                    },
                    scope: this,
                    success: function(result, request){
                        this.ctxNode.getUI().removeClass("x-tree-node-loading");
                        // clear query to query server again and reload subfolders
                        this.folderStore.resetQueryAndRemoveRecords('parent_path', (folder ? folder.get('path') : '/') + account.id);
                        this.ctxNode.reload(function(callback) {
                            this.selectInbox(account);
                        }, this);
                    },
                    failure: function() {
                        this.ctxNode.getUI().removeClass("x-tree-node-loading");
                    }
                });
            }
        }
    };
    
    // mutual config options
    var config = {
        nodeName: this.app.i18n.n_('Folder', 'Folders', 1),
        scope: this,
        backend: 'Felamimail',
        backendModel: 'Folder'
    };
    
    // system folder ctx menu
    config.actions = [markFolderSeenAction, 'add'];
    this.contextMenuSystemFolder = Tine.widgets.tree.ContextMenu.getMenu(config);
    
    // user folder ctx menu
    config.actions = [markFolderSeenAction, 'add', 'rename', 'delete'];
    this.contextMenuUserFolder = Tine.widgets.tree.ContextMenu.getMenu(config);
    
    // trash ctx menu
    config.actions = [markFolderSeenAction, 'add', emptyFolderAction];
    this.contextMenuTrash = Tine.widgets.tree.ContextMenu.getMenu(config);
    
    // account ctx menu
    this.contextMenuAccount = Tine.widgets.tree.ContextMenu.getMenu({
        nodeName: this.app.i18n.n_('Account', 'Accounts', 1),
        actions: [addFolderToRootAction, updateFolderCacheAction, editVacationAction, editRulesAction, editAccountAction, 'delete'],
        scope: this,
        backend: 'Felamimail',
        backendModel: 'Account'
    });
    
    // context menu for unselectable folders (like public/shared namespace)
    config.actions = ['add'];
    this.unselectableFolder = Tine.widgets.tree.ContextMenu.getMenu(config);
};
/*
 * Tine 2.0
 * 
 * @package     Felamimail
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2009-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 */
 
/**
 * @namespace   Tine.Felamimail
 * @class       Tine.Felamimail.TreeLoader
 * @extends     Tine.widgets.tree.Loader
 * 
 * <p>Felamimail Account/Folder Tree Loader</p>
 * <p>
 * </p>
 * 
 * @author      Philipp Schuele <p.schuele@metaways.de>
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * 
 * @param       {Object} config
 * @constructor
 * Create a new Tine.Felamimail.TreeLoader
 * 
 */
Tine.Felamimail.TreeLoader = Ext.extend(Tine.widgets.tree.Loader, {
    
    /**
     * request data
     * 
     * @param {Ext.tree.TreeNode} node
     * @param {Function} callback Function to call after the node has been loaded. The
     * function is passed the TreeNode which was requested to be loaded.
     * @param (Object) scope The cope (this reference) in which the callback is executed.
     * defaults to the loaded TreeNode.
     */
    requestData : function(node, callback, scope){
        
        if(this.fireEvent("beforeload", this, node, callback) !== false) {
            var fstore = Tine.Tinebase.appMgr.get('Felamimail').getFolderStore(),
                folder = fstore.getById(node.attributes.folder_id),
                path = (folder) ? folder.get('path') : node.attributes.path;
            
            // we need to call doQuery fn from store to transparently do async request
            fstore.asyncQuery('parent_path', path, function(node, callback, scope, data) {
                if (data) {
                    node.beginUpdate();
                    data.each(function(folderRecord) {
                        var n = this.createNode(folderRecord.copy().data);
                        if (n) {
                            node.appendChild(n);
                        }
                    }, this);
                    node.endUpdate();
                }
                this.runCallback(callback, scope || node, [node]);
            }, [node, callback, scope], this, fstore);
            
        } else {
            // if the load is cancelled, make sure we notify
            // the node that we are done
            this.runCallback(callback, scope || node, []);
        }
    },
    
    /**
     * inspectCreateNode
     * 
     * @private
     */
    inspectCreateNode: function(attr) {
        var account = Tine.Tinebase.appMgr.get('Felamimail').getAccountStore().getById(attr.account_id);
        
        // NOTE cweiss 2010-06-15 this has to be precomputed on server side!
        attr.has_children = (account && account.get('has_children_support')) ? attr.has_children : true;
        if (attr.has_children == "0") {
            attr.has_children = false;
        }
        
        Ext.apply(attr, {
            leaf: !attr.has_children,
            expandable: attr.has_children,
            cls: 'x-tree-node-collapsed',
            folder_id: attr.id,
            folderNode: true,
            allowDrop: true,
            text: this.app.i18n._hidden(attr.localname)
        });
        
        // show standard folders icons 
        if (account) {
            if (account.get('trash_folder') === attr.globalname) {
                if (attr.cache_totalcount > 0) {
                    attr.cls = 'felamimail-node-trash-full';
                } else {
                    attr.cls = 'felamimail-node-trash';
                }
            }
            if (account.get('sent_folder') === attr.globalname) {
                attr.cls = 'felamimail-node-sent';
            }
            if (account.get('drafts_folder') === attr.globalname) {
                attr.cls = 'felamimail-node-drafts';
            }
            if (account.get('templates_folder') === attr.globalname) {
                attr.cls = 'felamimail-node-templates';
            }
        }
        if (attr.globalname.match(/^inbox$/i)) {
            attr.cls = 'felamimail-node-inbox';
            attr.text = this.app.i18n._hidden('INBOX');
        }
        if (attr.globalname.match(/^junk$/i)) {
            attr.cls = 'felamimail-node-junk';
        }

        if (! attr.is_selectable) {
            attr.cls = 'felamimail-node-unselectable';
        }
    }
});
/*
 * Tine 2.0
 * 
 * @package     Felamimail
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2009-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */
 
Ext.namespace('Tine.Felamimail');

/**
 * @namespace   Tine.Felamimail
 * @class       Tine.Felamimail.FilterPanel
 * @extends     Tine.widgets.persistentfilter.PickerPanel
 * 
 * <p>Felamimail Favorites Panel</p>
 * 
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * 
 * @param       {Object} config
 * @constructor
 * Create a new Tine.Felamimail.FilterPanel
 */
Tine.Felamimail.FilterPanel = Ext.extend(Tine.widgets.persistentfilter.PickerPanel, {
    filterModel: 'Felamimail_Model_MessageFilter'
});

/**
 * @namespace   Tine.Felamimail
 * @class       Tine.Felamimail.TreePanel
 * @extends     Ext.tree.TreePanel
 * 
 * <p>Account/Folder Tree Panel</p>
 * <p>Tree of Accounts with folders</p>
 * <pre>
 * low priority:
 * TODO         make inbox/drafts/templates configurable in account
 * TODO         save tree state? @see http://examples.extjs.eu/?ex=treestate
 * TODO         disable delete action in account ctx menu if user has no manage_accounts right
 * </pre>
 * 
 * @author      Philipp Schuele <p.schuele@metaways.de>
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * 
 * @param       {Object} config
 * @constructor
 * Create a new Tine.Felamimail.TreePanel
 * 
 */
Tine.Felamimail.TreePanel = function(config) {
    Ext.apply(this, config);
    
    this.addEvents(
        /**
         * @event containeradd
         * Fires when a folder was added
         * @param {folder} the new folder
         */
        'containeradd',
        /**
         * @event containerdelete
         * Fires when a folder got deleted
         * @param {folder} the deleted folder
         */
        'containerdelete',
        /**
         * @event containerrename
         * Fires when a folder got renamed
         * @param {folder} the renamed folder
         */
        'containerrename'
    );
        
    Tine.Felamimail.TreePanel.superclass.constructor.call(this);
};

Ext.extend(Tine.Felamimail.TreePanel, Ext.tree.TreePanel, {
    
    /**
     * @property app
     * @type Tine.Felamimail.Application
     */
    app: null,
    
    /**
     * @property accountStore
     * @type Ext.data.JsonStore
     */
    accountStore: null,
    
    /**
     * @type Ext.data.JsonStore
     */
    folderStore: null,
    
    /**
     * @cfg {String} containerName
     */
    containerName: 'Folder',
    
    /**
     * TreePanel config
     * @private
     */
    rootVisible: false,
    
    /**
     * drag n drop
     */ 
    enableDrop: true,
    ddGroup: 'mailToTreeDDGroup',
    
    /**
     * @cfg
     */
    border: false,
    recordClass: Tine.Felamimail.Model.Account,
    filterMode: 'filterToolbar',
    
    /**
     * is needed by Tine.widgets.mainscreen.WestPanel to fake container tree panel
     */
    selectContainerPath: Ext.emptyFn,
    
    /**
     * init
     * @private
     */
    initComponent: function() {
        // get folder store
        this.folderStore = Tine.Tinebase.appMgr.get('Felamimail').getFolderStore();
        
        // init tree loader
        this.loader = new Tine.Felamimail.TreeLoader({
            folderStore: this.folderStore,
            app: this.app
        });

        // set the root node
        this.root = new Ext.tree.TreeNode({
            text: 'default',
            draggable: false,
            allowDrop: false,
            expanded: true,
            leaf: false,
            id: 'root'
        });
        
        // add account nodes
        this.initAccounts();
        
        // init drop zone
        this.dropConfig = {
            ddGroup: this.ddGroup || 'TreeDD',
            appendOnly: this.ddAppendOnly === true,
            notifyEnter : function() {this.isDropSensitive = true;}.createDelegate(this),
            notifyOut : function() {this.isDropSensitive = false;}.createDelegate(this),
            onNodeOver : function(n, dd, e, data) {
                var node = n.node;
                
                // auto node expand check (only for non-account nodes)
                if(!this.expandProcId && node.attributes.allowDrop && node.hasChildNodes() && !node.isExpanded()){
                    this.queueExpand(node);
                } else if (! node.attributes.allowDrop) {
                    this.cancelExpand();
                }
                return node.attributes.allowDrop ? 'tinebase-tree-drop-move' : false;
            },
            isValidDropPoint: function(n, dd, e, data){
                return n.node.attributes.allowDrop;
            }
        };
        
        // init selection model (multiselect)
        this.selModel = new Ext.tree.MultiSelectionModel({});
        
        // init context menu TODO use Ext.apply
        var initCtxMenu = Tine.Felamimail.setTreeContextMenus.createDelegate(this);
        initCtxMenu();
        
        // add listeners
        this.on('beforeclick', this.onBeforeClick, this);
        this.on('click', this.onClick, this);
        this.on('contextmenu', this.onContextMenu, this);
        this.on('beforenodedrop', this.onBeforenodedrop, this);
        this.on('append', this.onAppend, this);
        this.on('containeradd', this.onFolderAdd, this);
        this.on('containerrename', this.onFolderRename, this);
        this.on('containerdelete', this.onFolderDelete, this);
        this.selModel.on('selectionchange', this.onSelectionChange, this);
        this.folderStore.on('update', this.onUpdateFolderStore, this);
        
        // call parent::initComponent
        Tine.Felamimail.TreePanel.superclass.initComponent.call(this);
    },
    
    /**
     * add accounts from registry as nodes to root node
     * @private
     */
    initAccounts: function() {
        this.accountStore = this.app.getAccountStore();
        this.accountStore.each(this.addAccount, this);
        this.accountStore.on('update', this.onAccountUpdate, this);
    },
    
    /**
     * init extra tool tips
     */
    initToolTips: function() {
        this.folderTip = new Ext.ToolTip({
            target: this.getEl(),
            delegate: 'a.x-tree-node-anchor',
            renderTo: document.body,
            listeners: {beforeshow: this.updateFolderTip.createDelegate(this)}
        });
        
        this.folderProgressTip = new Ext.ToolTip({
            target: this.getEl(),
            delegate: '.felamimail-node-statusbox-progress',
            renderTo: document.body,
            listeners: {beforeshow: this.updateProgressTip.createDelegate(this)}
        });
        
        this.folderUnreadTip = new Ext.ToolTip({
            target: this.getEl(),
            delegate: '.felamimail-node-statusbox-unread',
            renderTo: document.body,
            listeners: {beforeshow: this.updateUnreadTip.createDelegate(this)}
        });
    },
    
    /**
     * called when tree selection changes
     * 
     * @param {} sm
     * @param {} node
     */
    onSelectionChange: function(sm, nodes) {
        if (this.filterMode == 'gridFilter' && this.filterPlugin) {
            this.filterPlugin.onFilterChange();
        }
        if (this.filterMode == 'filterToolbar' && this.filterPlugin) {
            
            // get filterToolbar
            var ftb = this.filterPlugin.getGridPanel().filterToolbar;
            // in case of filterPanel
            ftb = ftb.activeFilterPanel ? ftb.activeFilterPanel : ftb;
            
            if (! ftb.rendered) {
                this.onSelectionChange.defer(150, this, [sm, nodes]);
                return;
            }
            
            // remove path filter
            ftb.supressEvents = true;
            ftb.filterStore.each(function(filter) {
                var field = filter.get('field');
                if (field === 'path') {
                    ftb.deleteFilter(filter);
                }
            }, this);
            ftb.supressEvents = false;
            
            // set ftb filters according to tree selection
            var filter = this.getFilterPlugin().getFilter();
            ftb.addFilter(new ftb.record(filter));
        
            ftb.onFiltertrigger();
            
            // finally select the selected node, as filtertrigger clears all selections
            sm.suspendEvents();
            Ext.each(nodes, function(node) {
                sm.select(node, Ext.EventObject, true);
            }, this);
            sm.resumeEvents();
        }
    },
    
    onFilterChange: Tine.widgets.container.TreePanel.prototype.onFilterChange,
    
   /**
     * returns a filter plugin to be used in a grid
     * @private
     */
    getFilterPlugin: function() {
        if (!this.filterPlugin) {
            this.filterPlugin = new Tine.widgets.tree.FilterPlugin({
                treePanel: this,
                field: 'path',
                nodeAttributeField: 'path',
                singleNodeOperator: 'in'
            });
        }
        
        return this.filterPlugin;
    },
    
    /**
     * convert containerPath to treePath
     * 
     * @param {String}  containerPath
     * @return {String} treePath
     */
    getTreePath: function(path) {
        return '/root' + path;
    },
    
    /**
     * @private
     * 
     * expand default account and select INBOX
     */
    afterRender: function() {
        Tine.Felamimail.TreePanel.superclass.afterRender.call(this);
        this.initToolTips();
        this.selectInbox();
        
        if (this.filterMode == 'filterToolbar' && this.filterPlugin) {
            this.filterPlugin.getGridPanel().filterToolbar.on('change', this.onFilterChange, this);
        }
    },
    
    /**
     * select inbox of account
     * 
     * @param {Record} account
     */
    selectInbox: function(account) {
        var accountId = (account) ? account.id : Tine.Felamimail.registry.get('preferences').get('defaultEmailAccount');
        
        this.expandPath('/root/' + accountId + '/', null, function(success, parentNode) {
            Ext.each(parentNode.childNodes, function(node) {
                if (Ext.util.Format.lowercase(node.attributes.localname) == 'inbox') {
                    node.select();
                    return false;
                }
            }, this);
        });
    },
    
    /**
     * called when an account record updates
     * 
     * @param {Ext.data.JsonStore} store
     * @param {Tine.Felamimail.Model.Account} record
     * @param {String} action
     */
    onAccountUpdate: function(store, record, action) {
        if (action === Ext.data.Record.EDIT) {
            this.updateAccountStatus(record);
        }
    },
    
    /**
     * on append node
     * 
     * render status box
     * 
     * @param {Tine.Felamimail.TreePanel} tree
     * @param {Ext.Tree.TreeNode} node
     * @param {Ext.Tree.TreeNode} appendedNode
     * @param {Number} index
     */
    onAppend: function(tree, node, appendedNode, index) {
        appendedNode.ui.render = appendedNode.ui.render.createSequence(function() {
            var app = Tine.Tinebase.appMgr.get('Felamimail'),
                folder = app.getFolderStore().getById(appendedNode.id);
                
            if (folder) {
                app.getMainScreen().getTreePanel().addStatusboxesToNodeUi(this);
                app.getMainScreen().getTreePanel().updateFolderStatus(folder);
            }
        }, appendedNode.ui);
    },
    
    /**
     * add status boxes
     * 
     * @param {Object} nodeUi
     */
    addStatusboxesToNodeUi: function(nodeUi) {
        Ext.DomHelper.insertAfter(nodeUi.elNode.lastChild, {tag: 'span', 'class': 'felamimail-node-statusbox', cn:[
            {'tag': 'img', 'src': Ext.BLANK_IMAGE_URL, 'class': 'felamimail-node-statusbox-progress'},
            {'tag': 'span', 'class': 'felamimail-node-statusbox-unread'}
        ]});
    },
    
    /**
     * on before click handler
     * - accounts are not clickable because fetching all messages of account is too expensive
     * - skip event for folders that are not selectable
     * 
     * @param {Ext.tree.AsyncTreeNode} node
     */
    onBeforeClick: function(node) {
        if (this.accountStore.getById(node.id) || ! this.app.getFolderStore().getById(node.id).get('is_selectable')) {
            return false;
        }
    },
    
    /**
     * on click handler
     * 
     * - expand node
     * - update filter toolbar of grid
     * - start check mails delayed task
     * 
     * @param {Ext.tree.AsyncTreeNode} node
     * @private
     */
    onClick: function(node) {
        if (node.expandable) {
            node.expand();
        }
        
        if (node.id && node.id != '/' && node.attributes.globalname != '') {
            var folder = this.app.getFolderStore().getById(node.id);
            this.app.checkMailsDelayedTask.delay(0);
        }
    },
    
    /**
     * show context menu for folder tree
     * 
     * items:
     * - create folder
     * - rename folder
     * - delete folder
     * - ...
     * 
     * @param {} node
     * @param {} event
     * @private
     */
    onContextMenu: function(node, event) {
        this.ctxNode = node;
        
        var folder = this.app.getFolderStore().getById(node.id),
            account = folder ? this.accountStore.getById(folder.get('account_id')) :
                               this.accountStore.getById(node.id);
        
        if (! folder) {
            // edit/remove account
            if (account.get('ns_personal') !== 'default') {
                this.contextMenuAccount.items.each(function(item) {
                    // check account personal namespace -> disable 'add folder' if namespace is other than root 
                    if (item.iconCls == 'action_add') {
                        item.setDisabled(account.get('ns_personal') != '');
                    }
                    // disable filter rules/vacation if no sieve hostname is set
                    if (item.iconCls == 'action_email_replyAll' || item.iconCls == 'action_email_forward') {
                        item.setDisabled(account.get('sieve_hostname') == null || account.get('sieve_hostname') == '');
                    }
                });
                
                this.contextMenuAccount.showAt(event.getXY());
            }
        } else {
            if (folder.get('globalname') === account.get('trash_folder') || folder.get('localname').match(/junk/i)) {
                this.contextMenuTrash.showAt(event.getXY());
            } else if (! folder.get('is_selectable')){
                this.unselectableFolder.showAt(event.getXY());
            } else if (folder.get('system_folder')) {
                this.contextMenuSystemFolder.showAt(event.getXY());
            } else {
                this.contextMenuUserFolder.showAt(event.getXY());
            }
        }
    },
    
    /**
     * mail(s) got dropped on node
     * 
     * @param {Object} dropEvent
     * @private
     */
    onBeforenodedrop: function(dropEvent) {
        var targetFolderId = dropEvent.target.attributes.folder_id,
            targetFolder = this.app.getFolderStore().getById(targetFolderId);
                
        this.app.getMainScreen().getCenterPanel().moveSelectedMessages(targetFolder, false);
        return true;
    },
    
    /**
     * cleanup on destruction
     */
    onDestroy: function() {
        this.folderStore.un('update', this.onUpdateFolderStore, this);
    },
    
    /**
     * folder store gets updated -> update tree nodes
     * 
     * @param {Tine.Felamimail.FolderStore} store
     * @param {Tine.Felamimail.Model.Folder} record
     * @param {String} operation
     */
    onUpdateFolderStore: function(store, record, operation) {
        if (operation === Ext.data.Record.EDIT) {
            this.updateFolderStatus(record);
        }
    },
    
    /**
     * add new folder to the store
     * 
     * @param {Object} folderData
     */
    onFolderAdd: function(folderData) {
        var recordData = Ext.copyTo({}, folderData, Tine.Felamimail.Model.Folder.getFieldNames());
        var newRecord = Tine.Felamimail.folderBackend.recordReader({responseText: Ext.util.JSON.encode(recordData)});
        
        Tine.log.debug('Added new folder:' + newRecord.get('globalname'));
        
        this.folderStore.add([newRecord]);
        this.initNewFolderNode(newRecord);
    },
    
    /**
     * init new folder node
     * 
     * @param {Tine.Felamimail.Model.Folder} newRecord
     */
    initNewFolderNode: function(newRecord) {
        // update paths in node
        var appendedNode = this.getNodeById(newRecord.id);
        
        if (! appendedNode) {
            // node is not yet rendered -> reload parent
            var parentId = newRecord.get('parent_path').split('/').pop(),
                parentNode = this.getNodeById(parentId);
            parentNode.reload(function() {
                this.initNewFolderNode(newRecord);
            }, this);
            return;
        }
        
        appendedNode.attributes.path = newRecord.get('path');
        appendedNode.attributes.parent_path = newRecord.get('parent_path');
        
        // add unreadcount/progress/tooltip
        this.addStatusboxesToNodeUi(appendedNode.ui);
        this.updateFolderStatus(newRecord);
    },

    /**
     * rename folder in the store
     * 
     * @param {Object} folderData
     */
    onFolderRename: function(folderData) {
        var record = this.folderStore.getById(folderData.id);
        record.set('globalname', folderData.globalname);
        record.set('localname', folderData.localname);
        
        Tine.log.debug('Renamed folder:' + record.get('globalname'));
    },
        
    /**
     * remove deleted folder from the store
     * 
     * @param {Object} folderData
     */
    onFolderDelete: function(folderData) {
        // if we deleted account, remove it from account store
        if (folderData.record && folderData.record.modelName === 'Account') {
            this.accountStore.remove(this.accountStore.getById(folderData.id));
        }
        
        this.folderStore.remove(this.folderStore.getById(folderData.id));
    },
    
    /**
     * returns tree node id the given el is child of
     * 
     * @param  {HTMLElement} el
     * @return {String}
     */
    getElsParentsNodeId: function(el) {
        return Ext.fly(el, '_treeEvents').up('div[class^=x-tree-node-el]').getAttribute('tree-node-id', 'ext');
    },
    
    /**
     * updates account status icon in this tree
     * 
     * @param {Tine.Felamimail.Model.Account} account
     */
    updateAccountStatus: function(account) {
        var imapStatus = account.get('imap_status'),
            node = this.getNodeById(account.id),
            ui = node ? node.getUI() : null,
            nodeEl = ui ? ui.getEl() : null;
            
        Tine.log.info('Account ' + account.get('name') + ' updated with imap_status: ' + imapStatus);
        if (node && node.ui.rendered) {
            var statusEl = Ext.get(Ext.DomQuery.selectNode('span[class=felamimail-node-accountfailure]', nodeEl));
            if (! statusEl) {
                // create statusEl on the fly
                statusEl = Ext.DomHelper.insertAfter(ui.elNode.lastChild, {'tag': 'span', 'class': 'felamimail-node-accountfailure'}, true);
                statusEl.on('click', function() {
                    Tine.Felamimail.folderBackend.handleRequestException(account.getLastIMAPException());
                }, this);
            }
            
            statusEl.setVisible(imapStatus === 'failure');
        }
    },
    
    /**
     * updates folder status icons/info in this tree
     * 
     * @param {Tine.Felamimail.Model.Folder} folder
     */
    updateFolderStatus: function(folder) {
        var unreadcount = folder.get('cache_unreadcount'),
            progress    = Math.round(folder.get('cache_job_actions_done') / folder.get('cache_job_actions_est') * 10) * 10,
            node        = this.getNodeById(folder.id),
            ui = node ? node.getUI() : null,
            nodeEl = ui ? ui.getEl() : null,
            cacheStatus = folder.get('cache_status'),
            lastCacheStatus = folder.modified ? folder.modified.cache_status : null,
            isSelected = folder.isCurrentSelection();

        this.setUnreadClass(folder.id);
            
        if (node && node.ui.rendered) {
            var domNode = Ext.DomQuery.selectNode('span[class=felamimail-node-statusbox-unread]', nodeEl);
            if (domNode) {
                
                // update unreadcount + visibity
                Ext.fly(domNode).update(unreadcount).setVisible(unreadcount > 0);
                
                // update progress
                var progressEl = Ext.get(Ext.DomQuery.selectNode('img[class^=felamimail-node-statusbox-progress]', nodeEl));
                progressEl.removeClass(['felamimail-node-statusbox-progress-pie', 'felamimail-node-statusbox-progress-loading']);
                if (! Ext.isNumber(progress)) {
                    progressEl.setStyle('background-position', 0 + 'px');
                    progressEl.addClass('felamimail-node-statusbox-progress-loading');
                } else {
                    progressEl.setStyle('background-position', progress + '%');
                    progressEl.addClass('felamimail-node-statusbox-progress-pie');
                }
                progressEl.setVisible(isSelected && cacheStatus !== 'complete' && cacheStatus !== 'disconnect' && progress !== 100 && lastCacheStatus !== 'complete');
            }
        }
    },
    
    /**
     * set unread class of folder node and parents
     * 
     * @param {Tine.Felamimail.Model.Folder} folder
     * 
     * TODO make it work correctly for parents (use events) and activate again
     */
    setUnreadClass: function(folderId) {
        var folder              = this.app.getFolderStore().getById(folderId),
            node                = this.getNodeById(folderId),
            isUnread            = folder.get('cache_unreadcount') > 0,
            hasUnreadChildren   = folder.get('unread_children').length > 0;
            
        if (node && node.ui.rendered) {
            var ui = node.getUI();
            ui[(isUnread || hasUnreadChildren) ? 'addClass' : 'removeClass']('felamimail-node-unread');
        }
        
        // get parent, update and call recursivly
//        var parentFolder = this.app.getFolderStore().getParent(folder);
//        if (parentFolder) {
//            // need to create a copy of the array here (and make sure it is unique)
//            var unreadChildren = Ext.unique(parentFolder.get('unread_children'));
//                
//            if (isUnread || hasUnreadChildren) {
//                unreadChildren.push(folderId);
//            } else {
//                unreadChildren.remove(folderId);
//            }
//            parentFolder.set('unread_children', unreadChildren);
//            this.setUnreadClass(parentFolder.id);
//        }
    },
    
    /**
     * updates the given tip
     * @param {Ext.Tooltip} tip
     */
    updateFolderTip: function(tip) {
        var folderId = this.getElsParentsNodeId(tip.triggerElement),
            folder = this.app.getFolderStore().getById(folderId),
            account = this.accountStore.getById(folderId);
            
        if (folder && !this.isDropSensitive) {
            var info = [
                '<table>',
                    '<tr>',
                        '<td>', this.app.i18n._('Total Messages:'), '</td>',
                        '<td>', folder.get('cache_totalcount'), '</td>',
                    '</tr>',
                    '<tr>',
                        '<td>', this.app.i18n._('Unread Messages:'), '</td>',
                        '<td>', folder.get('cache_unreadcount'), '</td>',
                    '</tr>',
                    '<tr>',
                        '<td>', this.app.i18n._('Name on Server:'), '</td>',
                        '<td>', folder.get('globalname'), '</td>',
                    '</tr>',
                    '<tr>',
                        '<td>', this.app.i18n._('Last update:'), '</td>',
                        '<td>', Tine.Tinebase.common.dateTimeRenderer(folder.get('client_access_time')), '</td>',
                    '</tr>',
                '</table>'
            ];
            tip.body.dom.innerHTML = info.join('');
        } else {
            return false;
        }
    },
    
    /**
     * updates the given tip
     * @param {Ext.Tooltip} tip
     */
    updateProgressTip: function(tip) {
        var folderId = this.getElsParentsNodeId(tip.triggerElement),
            folder = this.app.getFolderStore().getById(folderId),
            progress = Math.round(folder.get('cache_job_actions_done') / folder.get('cache_job_actions_est') * 100);
        if (! this.isDropSensitive) {
            tip.body.dom.innerHTML = String.format(this.app.i18n._('Fetching messages... ({0}%% done)'), progress);
        } else {
            return false;
        }
    },
    
    /**
     * updates the given tip
     * @param {Ext.Tooltip} tip
     */
    updateUnreadTip: function(tip) {
        var folderId = this.getElsParentsNodeId(tip.triggerElement),
            folder = this.app.getFolderStore().getById(folderId),
            count = folder.get('cache_unreadcount');
            
        if (! this.isDropSensitive) {
            tip.body.dom.innerHTML = String.format(this.app.i18n.ngettext('{0} unread message', '{0} unread messages', count), count);
        } else {
            return false;
        }
    },
    
    /**
     * decrement unread count of currently selected folder
     */
    decrementCurrentUnreadCount: function() {
        var store  = Tine.Tinebase.appMgr.get('Felamimail').getFolderStore(),
            node   = this.getSelectionModel().getSelectedNode(),
            folder = node ? store.getById(node.id) : null;
            
        if (folder) {
            folder.set('cache_unreadcount', parseInt(folder.get('cache_unreadcount'), 10) -1);
            folder.commit();
        }
    },
    
    /**
     * add account record to root node
     * 
     * @param {Tine.Felamimail.Model.Account} record
     */
    addAccount: function(record) {
        
        var node = new Ext.tree.AsyncTreeNode({
            id: record.data.id,
            path: '/' + record.data.id,
            record: record,
            globalname: '',
            draggable: false,
            allowDrop: false,
            expanded: false,
            text: Ext.util.Format.htmlEncode(record.get('name')),
            qtip: Tine.Tinebase.common.doubleEncode(record.get('host')),
            leaf: false,
            cls: 'felamimail-node-account',
            delimiter: record.get('delimiter'),
            ns_personal: record.get('ns_personal'),
            account_id: record.data.id,
            listeners: {
                scope: this,
                load: function(node) {
                    var account = this.accountStore.getById(node.id);
                    this.updateAccountStatus(account);
                }
            }
        });
        
        // we don't want appending folder effects
        this.suspendEvents();
        this.root.appendChild(node);
        this.resumeEvents();
    },
    
    /**
     * get active account by checking selected node
     * @return Tine.Felamimail.Model.Account
     */
    getActiveAccount: function() {
        var result = null;
        var node = this.getSelectionModel().getSelectedNode();
        if (node) {
            var accountId = node.attributes.account_id;
            result = this.accountStore.getById(accountId);
        }
        
        return result;
    }
});
/*
 * Tine 2.0
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 * @copyright   Copyright (c) 2011 Metaways Infosystems GmbH (http://www.metaways.de)
 */
Ext.ns('Tine.Felamimail');

/**
 * display panel item registry per MIME type
 * 
 * @namespace   Tine.Felamimail
 * @class       Tine.Felamimail.MimeDisplayManager
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 * @singleton
 */
Tine.Felamimail.MimeDisplayManager = function() {
    var items = {},
        alternatives = {};
    
    return {
        /**
         * creates a new details panel for given mimeType
         * 
         * @param {String} mimeType
         * @return {Tine.widgets.grid.DisplayPanel}
         */
        create: function(mimeType, config) {
            var c = this.get(mimeType);
            
            if (! c) {
                throw new Ext.Error('No details panel registered for ' + mimeType);
            }
            
            return new c(config || {});
        },
        
        /**
         * returns basic form of MIME type if a display panel is registered for it
         * 
         * @param {String} mimeType
         * @return {String}
         */
        getMainType: function(mimeType) {
            var mainType = alternatives[mimeType];
            
            return items.hasOwnProperty(mainType) ? mainType : null;
        },
        
        /**
         * returns Display Panel for given MIME type
         * 
         * @param  {String} mimeType
         * @return {Function} consturctor function of a Tine.widgets.grid.DisplayPanel 
         */
        get: function(mimeType) {
            var mainType = this.getMainType(mimeType);
            
            return mainType ? items[mainType] : null;
        },
        
        /**
         * register Display Panel for given MIME type
         * 
         * @param {String/Array} mimeType
         * @param {Function} displayPanel consturctor function of a Tine.widgets.grid.DisplayPanel 
         * @param {Array} otherTypes
         */
        register: function(mimeType, displayPanel, otherTypes) {
            if (items.hasOwnProperty(mimeType)) {
                throw new Ext.Error('There is already an registration for ' + mimeType);
            }
            
            items[mimeType] = displayPanel;
            alternatives[mimeType] = mimeType;
            
            if (Ext.isArray(otherTypes)) {
                Ext.each(otherTypes, function(otherType) {
                    if (alternatives.hasOwnProperty(otherType)) {
                        throw new Ext.Error(alternatives[otherType] + ' is already registered as alternative for ' + otherType);
                    }
                    alternatives[otherType] = mimeType;
                }, this);
            }
        }
    };
}();/*
 * Tine 2.0
 * 
 * @package     Felamimail
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2009-2012 Metaways Infosystems GmbH (http://www.metaways.de)
 */
 
Ext.ns('Tine.Felamimail');

/**
 * @namespace   Tine.Felamimail
 * @class       Tine.Felamimail.GridDetailsPanel
 * @extends     Tine.widgets.grid.DetailsPanel
 * 
 * <p>Message Grid Details Panel</p>
 * <p>the details panel (shows message content)</p>
 * 
 * TODO         replace telephone numbers in emails with 'call contact' link
 * TODO         make only text body scrollable (headers should be always visible)
 * TODO         show image attachments inline
 * TODO         add 'download all' button
 * TODO         'from' to contact: check for duplicates
 * 
 * @param       {Object} config
 * @constructor
 * Create a new Tine.Felamimail.GridDetailsPanel
 */
 Tine.Felamimail.GridDetailsPanel = Ext.extend(Tine.widgets.grid.DetailsPanel, {
    
    /**
     * config
     * @private
     */
    defaultHeight: 350,
    currentId: null,
    record: null,
    app: null,
    i18n: null,
    
    fetchBodyTransactionId: null,
    
    /**
     * init
     * @private
     */
    initComponent: function() {
        this.initTemplate();
        this.initDefaultTemplate();
        //this.initTopToolbar();
        
        Tine.Felamimail.GridDetailsPanel.superclass.initComponent.call(this);
    },
    
    /**
     * use default Tpl for default and multi view
     */
    initDefaultTemplate: function() {
        this.defaultTpl = new Ext.XTemplate(
            '<div class="preview-panel-felamimail">',
                '<div class="preview-panel-felamimail-body">{[values ? values.msg : ""]}</div>',
            '</div>'
        );
    },
    
    /**
     * init bottom toolbar (needed for event invitations atm)
     * 
     * TODO add buttons (show header, add to addressbook, create filter, show images ...) here
     */
//    initTopToolbar: function() {
//        this.tbar = new Ext.Toolbar({
//            hidden: true,
//            items: []
//        });
//    },
    
    /**
     * add on click event after render
     * @private
     */
    afterRender: function() {
        Tine.Felamimail.GridDetailsPanel.superclass.afterRender.apply(this, arguments);
        this.body.on('click', this.onClick, this);
    },
    
    /**
     * get panel for single record details
     * 
     * @return {Ext.Panel}
     */
    getSingleRecordPanel: function() {
        if (! this.singleRecordPanel) {
            this.singleRecordPanel = new Ext.Panel({
                layout: 'vbox',
                layoutConfig: {
                    align:'stretch'
                },
                border: false,
                items: [
                    //this.getTopPanel(),
                    this.getMessageRecordPanel()
                    //this.getBottomPanel()
                ]
            });
        }
        return this.singleRecordPanel;
    },

    /**
     * get panel for single record details
     * 
     * @return {Ext.Panel}
     */
    getMessageRecordPanel: function() {
        if (! this.messageRecordPanel) {
            this.messageRecordPanel = new Ext.Panel({
                border: false,
                autoScroll: true,
                flex: 1
            });
        }
        return this.messageRecordPanel;
    },
    
    /**
     * (on) update details
     * 
     * @param {Tine.Felamimail.Model.Message} record
     * @param {String} body
     * @private
     */
    updateDetails: function(record, body) {
        if (record.id === this.currentId) {
            // nothing to do
        } else if (! record.bodyIsFetched()) {
            this.waitForContent(record, this.getMessageRecordPanel().body);
        } else if (record === this.record) {
            this.setTemplateContent(record, this.getMessageRecordPanel().body);
        }
    },
    
    /**
     * wait for body content
     * 
     * @param {Tine.Felamimail.Model.Message} record
     * @param {String} body
     */
    waitForContent: function(record, body) {
        if (! this.grid || this.grid.getSelectionModel().getCount() == 1) {
            this.refetchBody(record, {
                success: this.updateDetails.createDelegate(this, [record, body]),
                failure: function (exception) {
                    Tine.log.debug(exception);
                    this.getLoadMask().hide();
                    if (exception.code == 404) {
                        this.defaultTpl.overwrite(body, {msg: this.app.i18n._('Message not available.')});
                    } else {
                        Tine.Felamimail.messageBackend.handleRequestException(exception);
                    }
                },
                scope: this
            });
            this.defaultTpl.overwrite(body, {msg: ''});
            this.getLoadMask().show();
        } else {
            this.getLoadMask().hide();
        }
    },
    
    /**
     * refetch message body
     * 
     * @param {Tine.Felamimail.Model.Message} record
     * @param {Function} callback
     */
    refetchBody: function(record, callback) {
        // cancel old request first
        if (this.fetchBodyTransactionId && ! Tine.Felamimail.messageBackend.isLoading(this.fetchBodyTransactionId)) {
            Tine.log.debug('Tine.Felamimail.GridDetailsPanel::refetchBody -> cancelling current fetchBody request.');
            Tine.Felamimail.messageBackend.abort(this.fetchBodyTransactionId);
        }
        Tine.log.debug('Tine.Felamimail.GridDetailsPanel::refetchBody -> calling fetchBody');
        this.fetchBodyTransactionId = Tine.Felamimail.messageBackend.fetchBody(record, callback);
    },
    
    /**
     * overwrite template with (body) content
     * 
     * @param {Tine.Felamimail.Model.Message} record
     * @param {String} body
     * 
     * TODO allow other prepared parts than email invitations
     */
    setTemplateContent: function(record, body) {
        this.currentId = record.id;
        this.getLoadMask().hide();

        this.doLayout();

        this.tpl.overwrite(body, record.data);
        this.getEl().down('div').down('div').scrollTo('top', 0, false);
        
        if (this.record.get('preparedParts') && this.record.get('preparedParts').length > 0) {
            Tine.log.debug('Tine.Felamimail.GridDetailsPanel::setTemplateContent about to handle preparedParts');
            this.handlePreparedParts(record);
        }
    },
    
    /**
     * handle invitation messages (show top + bottom panels)
     * 
     * @param {Tine.Felamimail.Model.Message} record
     */
    handlePreparedParts: function(record) {
        var firstPreparedPart = this.record.get('preparedParts')[0],
            mimeType = String(firstPreparedPart.contentType).split(/[ ;]/)[0],
            mainType = Tine.Felamimail.MimeDisplayManager.getMainType(mimeType);
            
        if (! mainType) {
            Tine.log.info('Tine.Felamimail.GridDetailsPanel::handlePreparedParts nothing found to handle ' + mimeType);
            return;
        }
        
        var bodyEl = this.getMessageRecordPanel().getEl().query('div[class=preview-panel-felamimail-body]')[0],
            detailsPanel = Tine.Felamimail.MimeDisplayManager.create(mainType, {
                preparedPart: firstPreparedPart
            });
            
        // quick hack till we have a card body here 
        Ext.fly(bodyEl).update('');
        detailsPanel.render(bodyEl);
    },
    
    /**
     * init single message template (this.tpl)
     * @private
     */
    initTemplate: function() {
        
        this.tpl = new Ext.XTemplate(
            '<div class="preview-panel-felamimail">',
                '<div class="preview-panel-felamimail-headers">',
                    '<b>' + this.i18n._('Subject') + ':</b> {[this.encode(values.subject)]}<br/>',
                    '<b>' + this.i18n._('From') + ':</b>',
                    ' {[this.showFrom(values.from_email, values.from_name, "' + this.i18n._('Add') + '", "' 
                        + this.i18n._('Add contact to addressbook') + '")]}<br/>',
                    '<b>' + this.i18n._('Date') + ':</b> {[Tine.Tinebase.common.dateTimeRenderer(values.sent)]}',
                    '{[this.showRecipients(values.headers)]}',
                    '{[this.showHeaders("' + this.i18n._('Show or hide header information') + '")]}',
                '</div>',
                '<div class="preview-panel-felamimail-attachments">{[this.showAttachments(values.attachments, values)]}</div>',
                '<div class="preview-panel-felamimail-body">{[this.showBody(values.body, values)]}</div>',
            '</div>',{
            app: this.app,
            panel: this,
            encode: function(value) {
                if (value) {
                    var encoded = Ext.util.Format.htmlEncode(value);
                    encoded = Ext.util.Format.nl2br(encoded);
                    // it should be enough to replace only 2 or more spaces
                    encoded = encoded.replace(/ /g, '&nbsp;');
                    
                    return encoded;
                } else {
                    return '';
                }
                return value;
            },
            
            showFrom: function(email, name, addText, qtip) {
                if (name === null) {
                    return '';
                }
                
                var result = this.encode(name + ' <' + email + '>');
                
                // add link with 'add to contacts'
                var id = Ext.id() + ':' + email;
                
                var nameSplit = name.match(/^"*([^,^ ]+)(,*) *(.+)/i);
                var firstname = (nameSplit && nameSplit[1]) ? nameSplit[1] : '';
                var lastname = (nameSplit && nameSplit[3]) ? nameSplit[3] : '';
                if (nameSplit && nameSplit[2] == ',') {
                    firstname = lastname;
                    lastname = nameSplit[1];
                }
                
                id += Ext.util.Format.htmlEncode(':' + Ext.util.Format.trim(firstname) + ':' + Ext.util.Format.trim(lastname));
                result += ' <span ext:qtip="' + Tine.Tinebase.common.doubleEncode(qtip) + '" id="' + id + '" class="tinebase-addtocontacts-link">[+]</span>';
                return result;
            },
            
            showBody: function(body, messageData) {
                body = body || '';
                if (body) {
                    var account = this.app.getActiveAccount();
                    if (account && (account.get('display_format') == 'plain' || 
                        (account.get('display_format') == 'content_type' && messageData.body_content_type == 'text/plain'))
                    ) {
                        var width = this.panel.body.getWidth()-25,
                            height = this.panel.body.getHeight()-90,
                            id = Ext.id();
                            
                        if (height < 0) {
                            // sometimes the height is negative, fix this here
                            height = 500;
                        }
                            
                        body = '<textarea ' +
                            'style="width: ' + width + 'px; height: ' + height + 'px; " ' +
                            'autocomplete="off" id="' + id + '" name="body" class="x-form-textarea x-form-field x-ux-display-background-border" readonly="" >' +
                            body + '</textarea>';
                    }
                }
                return body;
            },
            
            showHeaders: function(qtip) {
                var result = ' <span ext:qtip="' + Tine.Tinebase.common.doubleEncode(qtip) + '" id="' + Ext.id() + ':show" class="tinebase-showheaders-link">[...]</span>';
                return result;
            },
            
            showRecipients: function(value) {
                if (value) {
                    var i18n = Tine.Tinebase.appMgr.get('Felamimail').i18n,
                        result = '';
                    for (header in value) {
                        if (value.hasOwnProperty(header) && (header == 'to' || header == 'cc' || header == 'bcc')) {
                            result += '<br/><b>' + i18n._hidden(Ext.util.Format.capitalize(header)) + ':</b> ' 
                                + Ext.util.Format.htmlEncode(value[header]);
                        }
                    }
                    return result;
                } else {
                    return '';
                }
            },
            
            showAttachments: function(attachments, messageData) {
                var result = (attachments.length > 0) ? '<b>' + this.app.i18n._('Attachments') + ':</b> ' : '';
                
                for (var i=0, id, cls; i < attachments.length; i++) {
                    result += '<span id="' + Ext.id() + ':' + i + '" class="tinebase-download-link">' 
                        + '<i>' + attachments[i].filename + '</i>' 
                        + ' (' + Ext.util.Format.fileSize(attachments[i].size) + ')</span> ';
                }
                
                return result;
            }
        });
    },
    
    /**
     * on click for attachment download / compose dlg / edit contact dlg
     * 
     * @param {} e
     * @private
     */
    onClick: function(e) {
        var selectors = [
            'span[class=tinebase-download-link]',
            'a[class=tinebase-email-link]',
            'span[class=tinebase-addtocontacts-link]',
            'span[class=tinebase-showheaders-link]'
        ];
        
        // find the correct target
        for (var i=0, target=null, selector=''; i < selectors.length; i++) {
            target = e.getTarget(selectors[i]);
            if (target) {
                selector = selectors[i];
                break;
            }
        }
        
        Tine.log.debug('Tine.Felamimail.GridDetailsPanel::onClick found target:"' + selector + '".');
        
        switch (selector) {
            case 'span[class=tinebase-download-link]':
                var idx = target.id.split(':')[1],
                    attachment = this.record.get('attachments')[idx];
                    
                if (! this.record.bodyIsFetched()) {
                    // sometimes there is bad timing and we do not have the attachments available -> refetch body
                    this.refetchBody(this.record, this.onClick.createDelegate(this, [e]));
                    return;
                }
                    
                // remove part id if set (that is the case in message/rfc822 attachments)
                var messageId = (this.record.id.match(/_/)) ? this.record.id.split('_')[0] : this.record.id;
                    
                if (attachment['content-type'] === 'message/rfc822') {
                    
                    Tine.log.debug('Tine.Felamimail.GridDetailsPanel::onClick openWindow for:"' + messageId + '_' + attachment.partId + '".');
                    // display message
                    Tine.Felamimail.MessageDisplayDialog.openWindow({
                        record: new Tine.Felamimail.Model.Message({
                            id: messageId + '_' + attachment.partId
                        })
                    });
                    
                } else {
                    // download attachment
                    new Ext.ux.file.Download({
                        params: {
                            requestType: 'HTTP',
                            method: 'Felamimail.downloadAttachment',
                            messageId: messageId,
                            partId: attachment.partId
                        }
                    }).start();
                }
                
                break;
                
            case 'a[class=tinebase-email-link]':
                // open compose dlg
                var email = target.id.split(':')[1];
                var defaults = Tine.Felamimail.Model.Message.getDefaultData();
                defaults.to = [email];
                defaults.body = Tine.Felamimail.getSignature();
                
                var record = new Tine.Felamimail.Model.Message(defaults, 0);
                var popupWindow = Tine.Felamimail.MessageEditDialog.openWindow({
                    record: record
                });
                break;
                
            case 'span[class=tinebase-addtocontacts-link]':
                // open edit contact dlg
            
                // check if addressbook app is available
                if (! Tine.Addressbook || ! Tine.Tinebase.common.hasRight('run', 'Addressbook')) {
                    return;
                }
            
                var id = Ext.util.Format.htmlDecode(target.id);
                var parts = id.split(':');
                
                var popupWindow = Tine.Addressbook.ContactEditDialog.openWindow({
                    listeners: {
                        scope: this,
                        'load': function(editdlg) {
                            editdlg.record.set('email', parts[1]);
                            editdlg.record.set('n_given', parts[2]);
                            editdlg.record.set('n_family', parts[3]);
                        }
                    }
                });
                
                break;
                
            case 'span[class=tinebase-showheaders-link]':
                // show headers
            
                var parts = target.id.split(':');
                var targetId = parts[0];
                var action = parts[1];
                
                var html = '';
                if (action == 'show') {
                    var recordHeaders = this.record.get('headers');
                    
                    for (header in recordHeaders) {
                        if (recordHeaders.hasOwnProperty(header) && (header != 'to' || header != 'cc' || header != 'bcc')) {
                            html += '<br/><b>' + header + ':</b> ' 
                                + Ext.util.Format.htmlEncode(recordHeaders[header]);
                        }
                    }
                
                    target.id = targetId + ':' + 'hide';
                    
                } else {
                    html = ' <span ext:qtip="' + Ext.util.Format.htmlEncode(this.i18n._('Show or hide header information')) + '" id="' 
                        + Ext.id() + ':show" class="tinebase-showheaders-link">[...]</span>'
                }
                
                target.innerHTML = html;
                
                break;
        }
    }
});
/*
 * Tine 2.0
 * 
 * @package     Felamimail
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2007-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 */
 
Ext.namespace('Tine.Felamimail');

/**
 * Message grid panel
 * 
 * @namespace   Tine.Felamimail
 * @class       Tine.Felamimail.GridPanel
 * @extends     Tine.widgets.grid.GridPanel
 * 
 * <p>Message Grid Panel</p>
 * <p><pre>
 * </pre></p>
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * 
 * @param       {Object} config
 * @constructor
 * Create a new Tine.Felamimail.GridPanel
 */
Tine.Felamimail.GridPanel = Ext.extend(Tine.widgets.grid.GridPanel, {
    /**
     * record class
     * @cfg {Tine.Felamimail.Model.Message} recordClass
     */
    recordClass: Tine.Felamimail.Model.Message,
    
    /**
     * message detail panel
     * 
     * @type Tine.Felamimail.GridDetailsPanel
     * @property detailsPanel
     */
    detailsPanel: null,
    
    /**
     * transaction id of current delete message request
     * @type Number
     */
    deleteTransactionId: null,
    
    /**
     * this is true if messages are moved/deleted
     * 
     * @type Boolean
     */
    movingOrDeleting: false,
    
    manualRefresh: false,
    
    /**
     * @private model cfg
     */
    evalGrants: false,
    filterSelectionDelete: true,
    // autoRefresh is done via onUpdateFolderStore
    autoRefreshInterval: false,
    
    /**
     * @private grid cfg
     */
    defaultSortInfo: {field: 'received', direction: 'DESC'},
    gridConfig: {
        autoExpandColumn: 'subject',
        // drag n dropfrom
        enableDragDrop: true,
        ddGroup: 'mailToTreeDDGroup'
    },
    // we don't want to update the preview panel on context menu
    updateDetailsPanelOnCtxMenu: false,
    
    /**
     * Return CSS class to apply to rows depending upon flags
     * - checks Flagged, Deleted and Seen
     * 
     * @param {Tine.Felamimail.Model.Message} record
     * @param {Integer} index
     * @return {String}
     */
    getViewRowClass: function(record, index) {
        var className = '';
        
        if (record.hasFlag('\\Flagged')) {
            className += ' flag_flagged';
        }
        if (record.hasFlag('\\Deleted')) {
            className += ' flag_deleted';
        }
        if (! record.hasFlag('\\Seen')) {
            className += ' flag_unread';
        }
        
        return className;
    },
    
    /**
     * init message grid
     * @private
     */
    initComponent: function() {
        
        this.app = Tine.Tinebase.appMgr.get('Felamimail');
        this.i18nEmptyText = this.app.i18n._('No Messages found or the cache is empty.');
        
        this.recordProxy = Tine.Felamimail.messageBackend;
        
        this.gridConfig.columns = this.getColumns();
        this.initFilterToolbar();
        this.initDetailsPanel();
        
        this.plugins = this.plugins || [];
        this.plugins.push(this.filterToolbar);
        
        this.pagingConfig = {
            doRefresh: this.doRefresh.createDelegate(this)
        };
        
        Tine.Felamimail.GridPanel.superclass.initComponent.call(this);
        this.grid.getSelectionModel().on('rowselect', this.onRowSelection, this);
        this.app.getFolderStore().on('update', this.onUpdateFolderStore, this);
        
        this.initPagingToolbar();
    },
    
    /**
     * add quota bar to paging toolbar
     */
    initPagingToolbar: function() {
        Ext.QuickTips.init();
        
        this.quotaBar = new Ext.ProgressBar({
            width: 120,
            height: 16,
            style: {
                marginTop: '1px'
            },
            text: this.app.i18n._('Quota usage')
        });
        this.pagingToolbar.insert(12, new Ext.Toolbar.Separator());
        this.pagingToolbar.insert(12, this.quotaBar);
        
        // NOTE: the Ext.progessbar has an ugly bug: it does not layout correctly when hidden
        //       so we need to listen when we get activated to relayout the progessbar
        Tine.Tinebase.appMgr.on('activate', function(app) {
            if (app.appName === 'Felamimail') {
                this.quotaBar.syncProgressBar();
                this.quotaBar.setWidth(this.quotaBar.getWidth());
            }
        }, this);
    },
    
    /**
     * cleanup on destruction
     */
    onDestroy: function() {
        this.app.getFolderStore().un('update', this.onUpdateFolderStore, this);
    },
    
    /**
     * folder store gets updated -> refresh grid if new messages arrived or messages have been removed
     * 
     * @param {Tine.Felamimail.FolderStore} store
     * @param {Tine.Felamimail.Model.Folder} record
     * @param {String} operation
     */
    onUpdateFolderStore: function(store, record, operation) {
        if (operation === Ext.data.Record.EDIT && record.isModified('cache_totalcount')) {
            var tree = this.app.getMainScreen().getTreePanel(),
                selectedNodes = (tree) ? tree.getSelectionModel().getSelectedNodes() : [];
            
            // only refresh if 1 or no messages are selected
            if (this.getGrid().getSelectionModel().getCount() <= 1) {
                var refresh = false;
                for (var i = 0; i < selectedNodes.length; i++) {
                    if (selectedNodes[i].id == record.id) {
                        refresh = true;
                        break;
                    }
                }
                
                // check if folder is in filter or allinboxes are selected and updated folder is an inbox
                if (! refresh) {
                    var filters = this.filterToolbar.getValue();
                    filters = filters.filters ? filter.filters : filters;
                    
                    for (var i = 0; i < filters.length; i++) {
                        if (filters[i].field == 'path' && filters[i].operator == 'in') {
                            if (filters[i].value.indexOf(record.get('path')) !== -1 || (filters[i].value.indexOf('/allinboxes') !== -1 && record.isInbox())) {
                                refresh = true;
                                break;
                            }
                        }
                    }
                }
                
                if (refresh && this.noDeleteRequestInProgress()) {
                    Tine.log.debug('Refresh grid because of folder update.');
                    this.loadGridData({
                        removeStrategy: 'keepBuffered',
                        autoRefresh: true
                    });
                }
            }
        }
    },
    
    /**
     * skip initial till we know the INBOX id
     */
    initialLoad: function() {
        var account = this.app.getActiveAccount(),
            accountId = account ? account.id : null,
            inbox = accountId ? this.app.getFolderStore().queryBy(function(record) {
                return record.get('account_id') === accountId && record.get('localname').match(/^inbox$/i);
            }, this).first() : null;
            
        if (! inbox) {
            this.initialLoad.defer(100, this, arguments);
            return;
        }
        
        return Tine.Felamimail.GridPanel.superclass.initialLoad.apply(this, arguments);
    },
    
    /**
     * init actions with actionToolbar, contextMenu and actionUpdater
     * 
     * @private
     */
    initActions: function() {

        this.action_write = new Ext.Action({
            requiredGrant: 'addGrant',
            actionType: 'add',
            text: this.app.i18n._('Compose'),
            handler: this.onMessageCompose.createDelegate(this),
            disabled: ! this.app.getActiveAccount(),
            iconCls: this.app.appName + 'IconCls'
        });

        this.action_reply = new Ext.Action({
            requiredGrant: 'readGrant',
            actionType: 'reply',
            text: this.app.i18n._('Reply'),
            handler: this.onMessageReplyTo.createDelegate(this, [false]),
            iconCls: 'action_email_reply',
            disabled: true
        });

        this.action_replyAll = new Ext.Action({
            requiredGrant: 'readGrant',
            actionType: 'replyAll',
            text: this.app.i18n._('Reply To All'),
            handler: this.onMessageReplyTo.createDelegate(this, [true]),
            iconCls: 'action_email_replyAll',
            disabled: true
        });

        this.action_forward = new Ext.Action({
            requiredGrant: 'readGrant',
            actionType: 'forward',
            text: this.app.i18n._('Forward'),
            handler: this.onMessageForward.createDelegate(this),
            iconCls: 'action_email_forward',
            disabled: true
        });

        this.action_flag = new Ext.Action({
            requiredGrant: 'readGrant',
            text: this.app.i18n._('Toggle highlighting'),
            handler: this.onToggleFlag.createDelegate(this, ['\\Flagged'], true),
            iconCls: 'action_email_flag',
            allowMultiple: true,
            disabled: true
        });
        
        this.action_markUnread = new Ext.Action({
            requiredGrant: 'readGrant',
            text: this.app.i18n._('Mark read/unread'),
            handler: this.onToggleFlag.createDelegate(this, ['\\Seen'], true),
            iconCls: 'action_mark_read',
            allowMultiple: true,
            disabled: true
        });
        
        this.action_deleteRecord = new Ext.Action({
            requiredGrant: 'deleteGrant',
            allowMultiple: true,
            singularText: this.app.i18n._('Delete'),
            pluralText: this.app.i18n._('Delete'),
            translationObject: this.i18nDeleteActionText ? this.app.i18n : Tine.Tinebase.translation,
            text: this.app.i18n._('Delete'),
            handler: this.onDeleteRecords,
            disabled: true,
            iconCls: 'action_delete',
            scope: this
        });
        
        this.action_addAccount = new Ext.Action({
            text: this.app.i18n._('Add Account'),
            handler: this.onAddAccount,
            iconCls: 'action_add',
            scope: this,
            disabled: ! Tine.Tinebase.common.hasRight('add_accounts', 'Felamimail')
        });
        this.action_printPreview = new Ext.Action({
            requiredGrant: 'readGrant',
            text: this.app.i18n._('Print Preview'),
            handler: this.onPrintPreview.createDelegate(this, []),
            disabled:true,
            iconCls:'action_printPreview',
            scope:this
        });
        this.action_print = new Ext.Action({
            requiredGrant: 'readGrant',
            text: this.app.i18n._('Print Message'),
            handler: this.onPrint.createDelegate(this, []),
            disabled:true,
            iconCls:'action_print',
            scope:this,
            menu:{
                items:[
                    this.action_printPreview
                ]
            }
        });
        this.actionUpdater.addActions([
//            this.action_write,
            this.action_deleteRecord,
            this.action_reply,
            this.action_replyAll,
            this.action_forward,
            this.action_flag,
            this.action_markUnread,
            this.action_addAccount,
            this.action_print,
            this.action_printPreview
        ]);
        
        this.contextMenu = new Ext.menu.Menu({
            items: [
                this.action_reply,
                this.action_replyAll,
                this.action_forward,
                this.action_flag,
                this.action_markUnread,
                this.action_deleteRecord
            ]
        });
    },
    
    /**
     * initialises filter toolbar
     * 
     * @private
     */
    initFilterToolbar: function() {
        this.filterToolbar = this.getFilterToolbar();
        this.filterToolbar.criteriaIgnores = [
            {field: 'query',     operator: 'contains',     value: ''},
            {field: 'id' },
            {field: 'path' }
        ];
    },    
    
    /**
     * the details panel (shows message content)
     * 
     * @private
     */
    initDetailsPanel: function() {
        this.detailsPanel = new Tine.Felamimail.GridDetailsPanel({
            gridpanel: this,
            grid: this,
            app: this.app,
            i18n: this.app.i18n
        });
    },
    
    /**
     * get action toolbar
     * 
     * @return {Ext.Toolbar}
     */
    getActionToolbar: function() {
        if (! this.actionToolbar) {
            this.actionToolbar = new Ext.Toolbar({
                defaults: {height: 55},
                items: [{
                    xtype: 'buttongroup',
                    columns: 8,
                    items: [
                        Ext.apply(new Ext.SplitButton(this.action_write), {
                            scale: 'medium',
                            rowspan: 2,
                            iconAlign: 'top',
                            arrowAlign:'right',
                            menu: new Ext.menu.Menu({
                                items: [],
                                plugins: [{
                                    ptype: 'ux.itemregistry',
                                    key:   'Tine.widgets.grid.GridPanel.addButton'
                                }]
                            })
                        }),
                        Ext.apply(new Ext.Button(this.action_deleteRecord), {
                            scale: 'medium',
                            rowspan: 2,
                            iconAlign: 'top'
                        }),
                        Ext.apply(new Ext.Button(this.action_reply), {
                            scale: 'medium',
                            rowspan: 2,
                            iconAlign: 'top'
                        }),
                        Ext.apply(new Ext.Button(this.action_replyAll), {
                            scale: 'medium',
                            rowspan: 2,
                            iconAlign: 'top'
                        }),
                        Ext.apply(new Ext.Button(this.action_forward), {
                            scale: 'medium',
                            rowspan: 2,
                            iconAlign: 'top'
                        }),
                        Ext.apply(new Ext.SplitButton(this.action_print), {
                            scale: 'medium',
                            rowspan: 2,
                            iconAlign:'top',
                            arrowAlign:'right'
                        }),
                        this.action_flag,
                        this.action_addAccount,
                        this.action_markUnread
                    ]
                }, this.getActionToolbarItems()]
            });
            
            if (this.filterToolbar && typeof this.filterToolbar.getQuickFilterField == 'function') {
                this.actionToolbar.add('->', this.filterToolbar.getQuickFilterField());
            }
        }
        
        return this.actionToolbar;
    },
    
    /**
     * returns cm
     * 
     * @private
     */
    getColumns: function(){
        return [{
            id: 'id',
            header: this.app.i18n._("Id"),
            width: 100,
            sortable: true,
            dataIndex: 'id',
            hidden: true
        }, {
            id: 'content_type',
            header: this.app.i18n._("Attachments"),
            width: 12,
            sortable: true,
            dataIndex: 'content_type',
            renderer: this.attachmentRenderer
        }, {
            id: 'flags',
            header: this.app.i18n._("Flags"),
            width: 24,
            sortable: true,
            dataIndex: 'flags',
            renderer: this.flagRenderer
        },{
            id: 'subject',
            header: this.app.i18n._("Subject"),
            width: 300,
            sortable: true,
            dataIndex: 'subject'
        },{
            id: 'from_email',
            header: this.app.i18n._("From (Email)"),
            width: 100,
            sortable: true,
            dataIndex: 'from_email'
        },{
            id: 'from_name',
            header: this.app.i18n._("From (Name)"),
            width: 100,
            sortable: true,
            dataIndex: 'from_name'
        },{
            id: 'sender',
            header: this.app.i18n._("Sender"),
            width: 100,
            sortable: true,
            dataIndex: 'sender',
            hidden: true
        },{
            id: 'to',
            header: this.app.i18n._("To"),
            width: 150,
            sortable: true,
            dataIndex: 'to',
            hidden: true
        },{
            id: 'sent',
            header: this.app.i18n._("Sent"),
            width: 100,
            sortable: true,
            dataIndex: 'sent',
            hidden: true,
            renderer: Tine.Tinebase.common.dateTimeRenderer
        },{
            id: 'received',
            header: this.app.i18n._("Received"),
            width: 100,
            sortable: true,
            dataIndex: 'received',
            renderer: Tine.Tinebase.common.dateTimeRenderer
        },{
            id: 'folder_id',
            header: this.app.i18n._("Folder"),
            width: 100,
            sortable: true,
            dataIndex: 'folder_id',
            hidden: true,
            renderer: this.accountAndFolderRenderer.createDelegate(this)
        },{
            id: 'size',
            header: this.app.i18n._("Size"),
            width: 80,
            sortable: true,
            dataIndex: 'size',
            hidden: true,
            renderer: Ext.util.Format.fileSize
        }];
    },
    
    /**
     * attachment column renderer
     * 
     * @param {String} value
     * @return {String}
     * @private
     */
    attachmentRenderer: function(value) {
        var result = '';
        
        if (value && value.match(/multipart\/mixed/)) {
            result = '<img class="FelamimailFlagIcon" src="images/oxygen/16x16/actions/attach.png">';
        }
        
        return result;
    },
    
    /**
     * get flag icon
     * 
     * @param {String} flags
     * @return {String}
     * @private
     * 
     * TODO  use spacer if first flag(s) is/are not set?
     */
    flagRenderer: function(value, metadata, record) {
        var icons = [],
            result = '';
            
        if (record.hasFlag('\\Answered')) {
            icons.push({src: 'images/oxygen/16x16/actions/mail-reply-sender.png', qtip: Ext.util.Format.htmlEncode(_('Answered'))});
        }   
        if (record.hasFlag('Passed')) {
            icons.push({src: 'images/oxygen/16x16/actions/mail-forward.png', qtip: Ext.util.Format.htmlEncode(_('Forwarded'))});
        }   
//        if (record.hasFlag('\\Recent')) {
//            icons.push({src: 'images/oxygen/16x16/actions/knewstuff.png', qtip: _('Recent')});
//        }   
        
        Ext.each(icons, function(icon) {
            result += '<img class="FelamimailFlagIcon" src="' + icon.src + '" ext:qtip="' + Tine.Tinebase.common.doubleEncode(icon.qtip) + '">';
        }, this);
        
        return result;
    },
    
    /**
     * returns account and folder globalname
     * 
     * @param {String} folderId
     * @param {Object} metadata
     * @param {Folder|Account} record
     * @return {String}
     */
    accountAndFolderRenderer: function(folderId, metadata, record) {
        var folderStore = this.app.getFolderStore(),
            account = this.app.getAccountStore().getById(record.get('account_id')),
            result = (account) ? account.get('name') : record.get('account_id'),
            folder = folderStore.getById(folderId);
        
        if (! folder) {
            folder = folderStore.getById(record.id);
            if (! folder) {
                // only account
                return (result) ? result : record.get('name');
            }
        }
            
        result += '/';
        if (folder) {
            result += folder.get('globalname');
        } else {
            result += folderId;
        }
            
        return result;
    },
    
    /**
     * executed when user clicks refresh btn
     */
    doRefresh: function() {
        var folder = this.getCurrentFolderFromTree(),
            refresh = this.pagingToolbar.refresh;
            
        // refresh is explicit
        this.editBuffer = [];
        this.manualRefresh = true;
        
        if (folder) {
            refresh.disable();
            Tine.log.info('User forced mail check for folder "' + folder.get('localname') + '"');
            this.app.checkMails(folder, function() {
                refresh.enable();
                this.manualRefresh = false;
            });
        } else {
            this.filterToolbar.onFilterChange();
        }
    },
    
    /**
     * get currently selected folder from tree
     * @return {Tine.Felamimail.Model.Folder}
     */
    getCurrentFolderFromTree: function() {
        var tree = this.app.getMainScreen().getTreePanel(),
            node = tree ? tree.getSelectionModel().getSelectedNode() : null,
            folder = node ? this.app.getFolderStore().getById(node.id) : null;
        
        return folder;
    },
    
    /**
     * delete messages handler
     * 
     * @return {void}
     */
    onDeleteRecords: function() {
        var account = this.app.getActiveAccount(),
            trashId = (account) ? account.getTrashFolderId() : null,
            trash = trashId ? this.app.getFolderStore().getById(trashId) : null,
            trashConfigured = (account.get('trash_folder'));
            
        return (trash && ! trash.isCurrentSelection()) || (! trash && trashConfigured) ? this.moveSelectedMessages(trash, true) : this.deleteSelectedMessages();
    },

    /**
     * permanently delete selected messages
     */
    deleteSelectedMessages: function() {
        this.moveOrDeleteMessages(null);
    },
    
    /**
     * move selected messages to given folder
     * 
     * @param {Tine.Felamimail.Model.Folder} folder
     * @param {Boolean} toTrash
     */
    moveSelectedMessages: function(folder, toTrash) {
        if (folder && folder.isCurrentSelection()) {
            // nothing to do ;-)
            return;
        }
        
        this.moveOrDeleteMessages(folder, toTrash);
    },
    
    /**
     * move (folder !== null) or delete selected messages 
     * 
     * @param {Tine.Felamimail.Model.Folder} folder
     * @param {Boolean} toTrash
     */
    moveOrDeleteMessages: function(folder, toTrash) {
        
        // this is needed to prevent grid reloads while messages are moved or deleted
        this.movingOrDeleting = true;
        
        var sm = this.getGrid().getSelectionModel(),
            filter = sm.getSelectionFilter(),
            msgsIds = [],
            foldersNeedUpdate = false;
        
        if (sm.isFilterSelect) {
            var msgs = this.getStore(),
                nextRecord = null;
        } else {
            var msgs = sm.getSelectionsCollection(),
                nextRecord = this.getNextMessage(msgs);
        }
        
        var increaseUnreadCountInTargetFolder = 0;
        msgs.each(function(msg) {
            var isSeen = msg.hasFlag('\\Seen'),
                currFolder = this.app.getFolderStore().getById(msg.get('folder_id')),
                diff = isSeen ? 0 : 1;
            
            if (currFolder) {
                currFolder.set('cache_unreadcount', currFolder.get('cache_unreadcount') - diff);
                currFolder.set('cache_totalcount', currFolder.get('cache_totalcount') - 1);
                if (sm.isFilterSelect && sm.getCount() > 50 && currFolder.get('cache_status') !== 'pending') {
                    Tine.log.debug('Tine.Felamimail.GridPanel::moveOrDeleteMessages - Set cache status to pending for folder ' + currFolder.get('globalname'));
                    currFolder.set('cache_status', 'pending');
                    foldersNeedUpdate = true;
                }
                currFolder.commit();
            }
            if (folder) {
                increaseUnreadCountInTargetFolder += diff;
            }
           
            msgsIds.push(msg.id);
            this.getStore().remove(msg);
        },  this);
        
        if (folder && increaseUnreadCountInTargetFolder > 0) {
            // update unread count of target folder (only when moving)
            folder.set('cache_unreadcount', folder.get('cache_unreadcount') + increaseUnreadCountInTargetFolder);
            if (foldersNeedUpdate) {
                Tine.log.debug('Tine.Felamimail.GridPanel::moveOrDeleteMessages - Set cache status to pending for target folder ' + folder.get('globalname'));
                folder.set('cache_status', 'pending');
            }
            folder.commit();
        }
        
        if (foldersNeedUpdate) {
            Tine.log.debug('Tine.Felamimail.GridPanel::moveOrDeleteMessages - update message cache for "pending" folders');
            this.app.checkMailsDelayedTask.delay(1000);
        }
        
        this.deleteQueue = this.deleteQueue.concat(msgsIds);
        this.pagingToolbar.refresh.disable();
        if (nextRecord !== null) {
            sm.selectRecords([nextRecord]);
        }
        
        var callbackFn = this.onAfterDelete.createDelegate(this, [msgsIds]);
        
        if (folder !== null || toTrash) {
            // move
            var targetFolderId = (toTrash) ? '_trash_' : folder.id;
            this.deleteTransactionId = Tine.Felamimail.messageBackend.moveMessages(filter, targetFolderId, {
                callback: callbackFn
            });
        } else {
            // delete
            this.deleteTransactionId = Tine.Felamimail.messageBackend.addFlags(filter, '\\Deleted', {
                callback: callbackFn
            });
        }
    },

    /**
     * get next message in grid
     * 
     * @param {Ext.util.MixedCollection} msgs
     * @return Tine.Felamimail.Model.Message
     */
    getNextMessage: function(msgs) {
        
        var nextRecord = null;
        
        if (msgs.getCount() == 1 && this.getStore().getCount() > 1) {
            // select next message (or previous if it was the last or BACKSPACE)
            var lastIdx = this.getStore().indexOf(msgs.last()),
                direction = Ext.EventObject.getKey() == Ext.EventObject.BACKSPACE ? -1 : +1;
            
            nextRecord = this.getStore().getAt(lastIdx + 1 * direction);
            if (! nextRecord) {
                nextRecord = this.getStore().getAt(lastIdx + (-1) * direction);
            }
        }
        
        return nextRecord;
    },
    
    /**
     * executed after a msg compose
     * 
     * @param {String} composedMsg
     * @param {String} action
     * @param {Array}  [affectedMsgs]  messages affected 
     * @param {String} [mode]
     */
    onAfterCompose: function(composedMsg, action, affectedMsgs, mode) {
        Tine.log.debug('Tine.Felamimail.GridPanel::onAfterCompose / arguments:');
        Tine.log.debug(arguments);

        // mark send folders cache status incomplete
        composedMsg = Ext.isString(composedMsg) ? new this.recordClass(Ext.decode(composedMsg)) : composedMsg;
        
        // NOTE: if affected messages is decoded, we need to fetch the originals out of our store
        if (Ext.isString(affectedMsgs)) {
            var msgs = [],
                store = this.getStore();
            Ext.each(Ext.decode(affectedMsgs), function(msgData) {
                var msg = store.getById(msgData.id);
                if (msg) {
                    msgs.push(msg);
                }
            }, this);
            affectedMsgs = msgs;
        }
        
        var composerAccount = this.app.getAccountStore().getById(composedMsg.get('account_id')),
            sendFolderId = composerAccount ? composerAccount.getSendFolderId() : null,
            sendFolder = sendFolderId ? this.app.getFolderStore().getById(sendFolderId) : null;
            
        if (sendFolder) {
            sendFolder.set('cache_status', 'incomplete');
        }
        
        if (Ext.isArray(affectedMsgs)) {
            Ext.each(affectedMsgs, function(msg) {
                if (['reply', 'forward'].indexOf(action) !== -1) {
                    msg.addFlag(action === 'reply' ? '\\Answered' : 'Passed');
                } else if (action == 'senddraft') {
                    this.deleteTransactionId = Tine.Felamimail.messageBackend.addFlags(msg.id, '\\Deleted', {
                        callback: this.onAfterDelete.createDelegate(this, [[msg.id]])
                    });
                }
            }, this);
        } 
    },
    
    /**
     * executed after msg delete
     * 
     * @param {Array} [ids]
     */
    onAfterDelete: function(ids) {
        this.deleteQueue = this.deleteQueue.diff(ids);
        this.editBuffer = this.editBuffer.diff(ids);
        
        this.movingOrDeleting = false;

        if (this.noDeleteRequestInProgress()) {
            Tine.log.debug('Loading grid data after delete.');
            this.loadGridData({
                removeStrategy: 'keepBuffered',
                autoRefresh: true
            });
        }
    },
    
    /**
     * check if delete/move action is running atm
     * 
     * @return {Boolean}
     */
    noDeleteRequestInProgress: function() {
        return (
            ! this.movingOrDeleting && 
            (! this.deleteTransactionId || ! Tine.Felamimail.messageBackend.isLoading(this.deleteTransactionId))
        );
    },
    
    /**
     * compose new message handler
     */
    onMessageCompose: function() {
        var activeAccount = Tine.Tinebase.appMgr.get('Felamimail').getActiveAccount();
        
        var win = Tine.Felamimail.MessageEditDialog.openWindow({
            accountId: activeAccount ? activeAccount.id : null,
            listeners: {
                'update': this.onAfterCompose.createDelegate(this, ['compose', []], 1)
            }
        });
    },
    
    /**
     * forward message(s) handler
     */
    onMessageForward: function() {
        var sm = this.getGrid().getSelectionModel(),
            msgs = sm.getSelections(),
            msgsData = [];
            
        Ext.each(msgs, function(msg) {msgsData.push(msg.data)}, this);
        
        if (sm.getCount() > 0) {
            var win = Tine.Felamimail.MessageEditDialog.openWindow({
                forwardMsgs : Ext.encode(msgsData),
                listeners: {
                    'update': this.onAfterCompose.createDelegate(this, ['forward', msgs], 1)
                }
            });
        }
    },
    
    /**
     * reply message handler
     * 
     * @param {bool} toAll
     */
    onMessageReplyTo: function(toAll) {
        var sm = this.getGrid().getSelectionModel(),
            msg = sm.getSelected();
            
        var win = Tine.Felamimail.MessageEditDialog.openWindow({
            replyTo : Ext.encode(msg.data),
            replyToAll: toAll,
            listeners: {
                'update': this.onAfterCompose.createDelegate(this, ['reply', [msg]], 1)
            }
        });
    },
    
    /**
     * called when a row gets selected
     * 
     * @param {SelectionModel} sm
     * @param {Number} rowIndex
     * @param {Tine.Felamimail.Model.Message} record
     * @param {Boolean} now
     */
    onRowSelection: function(sm, rowIndex, record, now) {
        if (! now) {
            return this.onRowSelection.defer(250, this, [sm, rowIndex, record, true]);
        }
        
        if (sm.getCount() == 1 && sm.isIdSelected(record.id) && !record.hasFlag('\\Seen')) {
            record.addFlag('\\Seen');
            Tine.Felamimail.messageBackend.addFlags(record.id, '\\Seen');
            this.app.getMainScreen().getTreePanel().decrementCurrentUnreadCount();
        }
    },
    
    /**
     * row doubleclick handler
     * 
     * - opens message edit dialog (if draft/template)
     * - opens message display dialog (everything else)
     * 
     * @param {Tine.Felamimail.GridPanel} grid
     * @param {Row} row
     * @param {Event} e
     */
    onRowDblClick: function(grid, row, e) {
        
        var record = this.grid.getSelectionModel().getSelected(),
            folder = this.app.getFolderStore().getById(record.get('folder_id')),
            account = this.app.getAccountStore().getById(folder.get('account_id')),
            action = (folder.get('globalname') == account.get('drafts_folder')) ? 'senddraft' :
                      folder.get('globalname') == account.get('templates_folder') ? 'sendtemplate' : null,
            win;
        
        // check folder to determine if mail should be opened in compose dlg
        if (action !== null) {
            win = Tine.Felamimail.MessageEditDialog.openWindow({
                draftOrTemplate: Ext.encode(record.data),
                listeners: {
                    scope: this,
                    'update': this.onAfterCompose.createDelegate(this, [action, [record]], 1)
                }
            });
        } else {
            win = Tine.Felamimail.MessageDisplayDialog.openWindow({
                record: record,
                listeners: {
                    scope: this,
                    'update': this.onAfterCompose.createDelegate(this, ['compose', []], 1),
                    'remove': this.onRemoveInDisplayDialog
                }
            });
        }
    },
    
    /**
     * message got removed in display dialog
     * 
     * @param {} msgData
     */
    onRemoveInDisplayDialog: function (msgData) {
        var msg = this.getStore().getById(Ext.decode(msgData).id),
            folderId = msg ? msg.get('folder_id') : null,
            folder = folderId ? this.app.getFolderStore().getById(folderId) : null,
            accountId = folder ? folder.get('account_id') : null,
            account = accountId ? this.app.getAccountStore().getById(accountId) : null;
            
        this.getStore().remove(msg);
        this.onAfterDelete(null);
    },    
    
    /**
     * called when the store gets updated
     * 
     * NOTE: we only allow updateing flags BUT the actual updating is done 
     *       directly from the UI fn's to support IMAP optimised bulk actions
     */
    onStoreUpdate: function(store, record, operation) {
        if (operation === Ext.data.Record.EDIT && record.isModified('flags')) {
            record.commit()
        }
    },
    
    /**
     * key down handler
     * 
     * @param {Event} e
     */
    onKeyDown: function(e) {
        if (e.ctrlKey) {
            switch (e.getKey()) {
                case e.N:
                case e.M:
                    this.onMessageCompose();
                    e.preventDefault();
                    break;
                case e.R:
                    this.onMessageReplyTo();
                    e.preventDefault();
                    break;
                case e.L:
                    this.onMessageForward();
                    e.preventDefault();
                    break;
            }
        }
        
        Tine.Felamimail.GridPanel.superclass.onKeyDown.call(this, e);
    },
    
    /**
     * toggle flagged status of mail(s)
     * - Flagged/Seen
     * 
     * @param {Button} button
     * @param {Event} event
     * @param {String} flag
     */
    onToggleFlag: function(btn, e, flag) {
        var sm = this.getGrid().getSelectionModel(),
            filter = sm.getSelectionFilter(),
            msgs = sm.isFilterSelect ? this.getStore() : sm.getSelectionsCollection(),
            flagCount = 0;
            
        // switch all msgs to one state -> toogle most of them
        msgs.each(function(msg) {
            flagCount += msg.hasFlag(flag) ? 1 : 0;
        });
        var action = flagCount >= Math.round(msgs.getCount()/2) ? 'clear' : 'add';
        
        // mark messages in UI and add to edit buffer
        msgs.each(function(msg) {
            // update unreadcount
            if (flag === '\\Seen') {
                var isSeen = msg.hasFlag('\\Seen'),
                    folder = this.app.getFolderStore().getById(msg.get('folder_id')),
                    diff = (action === 'clear' && isSeen) ? 1 :
                           (action === 'add' && ! isSeen) ? -1 : 0;
                
                if (folder) {
                    folder.set('cache_unreadcount', folder.get('cache_unreadcount') + diff);
                    if (sm.isFilterSelect && sm.getCount() > 50 && folder.get('cache_status') !== 'pending') {
                        Tine.log.debug('Tine.Felamimail.GridPanel::moveOrDeleteMessages - Set cache status to pending for folder ' + folder.get('globalname'));
                        folder.set('cache_status', 'pending');
                    }
                    folder.commit();
                }
            }
            
            msg[action + 'Flag'](flag);
            
            this.addToEditBuffer(msg);
        }, this);
        
        if (sm.isFilterSelect && sm.getCount() > 50) {
            Tine.log.debug('Tine.Felamimail.GridPanel::moveOrDeleteMessages - Update message cache for "pending" folders');
            this.app.checkMailsDelayedTask.delay(1000);
        }
        
        // do request
        Tine.Felamimail.messageBackend[action+ 'Flags'](filter, flag);
    },
    
    /**
     * called before store queries for data
     */
    onStoreBeforeload: function(store, options) {
        this.supr().onStoreBeforeload.apply(this, arguments);
        
        if (! Ext.isEmpty(this.deleteQueue)) {
            options.params.filter.push({field: 'id', operator: 'notin', value: this.deleteQueue});
        }
    },
    
    /**
     *  called after a new set of Records has been loaded
     *  
     * @param  {Ext.data.Store} this.store
     * @param  {Array}          loaded records
     * @param  {Array}          load options
     * @return {Void}
     */
    onStoreLoad: function(store, records, options) {
        this.supr().onStoreLoad.apply(this, arguments);
        
        Tine.log.debug('Tine.Felamimail.GridPanel::onStoreLoad(): store loaded new records.');
        
        var folder = this.getCurrentFolderFromTree();
        if (folder && records.length < folder.get('cache_totalcount')) {
            Tine.log.debug('Tine.Felamimail.GridPanel::onStoreLoad() - Count mismatch: got ' + records.length + ' records for folder ' + folder.get('globalname'));
            Tine.log.debug(folder);
            folder.set('cache_status', 'pending');
            folder.commit();
            this.app.checkMailsDelayedTask.delay(1000);
        }
        
        this.updateQuotaBar();
    },
    
    /**
     * update quotaBar / only do it if we have a path filter with a single account id
     * 
     * @param {Record} accountInbox
     */
    updateQuotaBar: function(accountInbox) {
        var accountId = this.extractAccountIdFromFilter();
        
        if (accountId === null) {
            Tine.log.debug('No or multiple account ids in filter. Resetting quota bar.');
            this.quotaBar.hide();
            return;
        }
            
        if (! accountInbox) {
            var accountInbox = this.app.getFolderStore().queryBy(function(folder) {
                return folder.isInbox() && (folder.get('account_id') == accountId);
            }, this).first();
        }
        if (accountInbox && parseInt(accountInbox.get('quota_limit'), 10) && accountId == accountInbox.get('account_id')) {
            Tine.log.debug('Showing quota info.');
            
            var limit = parseInt(accountInbox.get('quota_limit'), 10),
                usage = parseInt(accountInbox.get('quota_usage'), 10),
                left = limit - usage,
                percentage = Math.round(usage/limit * 100),
                text = String.format(this.app.i18n._('{0} %'), percentage);
            this.quotaBar.show();
            this.quotaBar.setWidth(75);
            this.quotaBar.updateProgress(usage/limit, text);
            
            Ext.QuickTips.register({
                target:  this.quotaBar,
                dismissDelay: 30000,
                title: this.app.i18n._('Your quota'),
                text: String.format(this.app.i18n._('{0} available (total: {1})'), 
                    Ext.util.Format.fileSize(left * 1024),
                    Ext.util.Format.fileSize(limit * 1024)
                ),
                width: 200
            });
        } else {
            Tine.log.debug('No account inbox found or no quota info found.');
            this.quotaBar.hide();
        }
    },
    
    /**
     * get account id from filter (only returns the id if a single account id was found)
     * 
     * @param {Array} filter
     * @return {String}
     */
    extractAccountIdFromFilter: function(filter) {
        if (! filter) {
            filter = this.filterToolbar.getValue();
        }
        
        // use first OR panel in case of filterPanel
        Ext.each(filter, function(filterData) {
            if (filterData.condition && filterData.condition == 'OR') {
                filter = filterData.filters[0].filters;
                return false;
            }
        }, this);
        
        // condition from filterPanel
        while (filter.filters || (Ext.isArray(filter) && filter.length > 0 && filter[0].filters)) {
            filter = (filter.filters) ? filter.filters : filter[0].filters;
        }
        
        var accountId = null, 
            filterAccountId = null,
            accountIdMatch = null;

        for (var i = 0; i < filter.length; i++) {
            if (filter[i].field == 'path' && filter[i].operator == 'in') {
                for (var j = 0; j < filter[i].value.length; j++) {
                    accountIdMatch = filter[i].value[j].match(/^\/([a-z0-9]*)/i);
                    if (accountIdMatch) {
                        filterAccountId = accountIdMatch[1];
                        if (accountId && accountId != filterAccountId) {
                            // multiple different account ids found!
                            return null;
                        } else {
                            accountId = filterAccountId;
                        }
                    }
                }
            }
        }
        
        return accountId;
    },
    
    /**
     * add new account button
     * 
     * @param {Button} button
     * @param {Event} event
     */
    onAddAccount: function(button, event) {
        var popupWindow = Tine.Felamimail.AccountEditDialog.openWindow({
            record: null,
            listeners: {
                scope: this,
                'update': function(record) {
                    var account = new Tine.Felamimail.Model.Account(Ext.util.JSON.decode(record));
                    
                    // add to registry
                    Tine.Felamimail.registry.get('preferences').replace('defaultEmailAccount', account.id);
                    // need to do this because store could be unitialized yet
                    var registryAccounts = Tine.Felamimail.registry.get('accounts');
                    registryAccounts.results.push(account.data);
                    registryAccounts.totalcount++;
                    Tine.Felamimail.registry.replace('accounts', registryAccounts);
                    
                    // add to tree / store
                    var treePanel = this.app.getMainScreen().getTreePanel();
                    treePanel.addAccount(account);
                    treePanel.accountStore.add([account]);
                }
            }
        });
    },
    
    /**
     * print handler
     * 
     * @todo move this to Ext.ux.Printer as iframe driver
     * @param {Tine.Felamimail.GridDetailsPanel} details panel [optional]
     */
    onPrint: function(detailsPanel) {
        var id = Ext.id(),
            doc = document,
            frame = doc.createElement('iframe');
            
        Ext.fly(frame).set({
            id: id,
            name: id,
            style: {
                position: 'absolute',
                width: '210mm',
                height: '297mm',
                top: '-10000px', 
                left: '-10000px'
            }
        });
        
        doc.body.appendChild(frame);

        Ext.fly(frame).set({
           src : Ext.SSL_SECURE_URL
        });

        var doc = frame.contentWindow.document || frame.contentDocument || WINDOW.frames[id].document,
            content = this.getDetailsPanelContentForPrinting(detailsPanel || this.detailsPanel);
            
        doc.open();
        doc.write(content);
        doc.close();
        
        frame.contentWindow.focus();
        frame.contentWindow.print();
        
        // removeing frame chrashes chrome
//        setTimeout(function(){Ext.removeNode(frame);}, 100);
    },
    
    /**
     * get detail panel content
     * 
     * @param {Tine.Felamimail.GridDetailsPanel} details panel
     * @return {String}
     */
    getDetailsPanelContentForPrinting: function(detailsPanel) {
        // TODO somehow we have two <div class="preview-panel-felamimail"> -> we need to fix that and get the first element found
        var detailsPanels = detailsPanel.getEl().query('.preview-panel-felamimail');
        var detailsPanelContent = (detailsPanels.length > 1) ? detailsPanels[1].innerHTML : detailsPanels[0].innerHTML;
        
        var buffer = '<html><head>';
        buffer += '<title>' + this.app.i18n._('Print Preview') + '</title>';
        buffer += '</head><body>';
        buffer += detailsPanelContent;
        buffer += '</body></html>';
        
        return buffer;
    },
    
    /**
     * print preview handler
     * 
     * @param {Tine.Felamimail.GridDetailsPanel} details panel [optional]
     */
    onPrintPreview: function(detailsPanel) {
        var content = this.getDetailsPanelContentForPrinting(detailsPanel || this.detailsPanel);
        
        var win = window.open('about:blank',this.app.i18n._('Print Preview'),'width=500,height=500,scrollbars=yes,toolbar=yes,status=yes,menubar=yes');
        win.document.open()
        win.document.write(content);
        win.document.close();
        win.focus();
    },
    
    /**
     * format headers
     * 
     * @param {Object} headers
     * @param {Bool} ellipsis
     * @param {Bool} onlyImportant
     * @return {String}
     */
    formatHeaders: function(headers, ellipsis, onlyImportant) {
        var result = '';
        for (header in headers) {
            if (headers.hasOwnProperty(header) && 
                    (! onlyImportant || header == 'from' || header == 'to' || header == 'cc' || header == 'subject' || header == 'date')) 
            {
                result += '<b>' + header + ':</b> ' 
                    + Ext.util.Format.htmlEncode(
                        (ellipsis) 
                            ? Ext.util.Format.ellipsis(headers[header], 40)
                            : headers[header]
                    ) + '<br/>';
            }
        }
        return result;
    }    
});
/*
 * Tine 2.0
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2009-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 */
 
Ext.ns('Tine.Felamimail')


Tine.Felamimail.MessageDisplayDialog = Ext.extend(Tine.Felamimail.GridDetailsPanel ,{
    /**
     * @cfg {Tine.Felamimail.Model.Message}
     */
    record: null,
    
    autoScroll: false,
    
    initComponent: function() {
        this.addEvents('remove');
        
        this.app = Tine.Tinebase.appMgr.get('Felamimail');
        this.i18n = this.app.i18n;
        
        // trick onPrint/onPrintPreview
        this.detailsPanel = this;
        
        this.initActions();
        this.initToolbar();
        
        this.supr().initComponent.apply(this, arguments);
    },
    
    /**
     * init actions
     */
    initActions: function() {
        this.action_deleteRecord = new Ext.Action({
            text: this.app.i18n._('Delete'),
            handler: this.onMessageDelete.createDelegate(this, [false]),
            iconCls: 'action_delete',
            disabled: this.record.id.match(/_/)
        });
        
        this.action_reply = new Ext.Action({
            text: this.app.i18n._('Reply'),
            handler: this.onMessageReplyTo.createDelegate(this, [false]),
            iconCls: 'action_email_reply'
        });

        this.action_replyAll = new Ext.Action({
            text: this.app.i18n._('Reply To All'),
            handler: this.onMessageReplyTo.createDelegate(this, [true]),
            iconCls: 'action_email_replyAll'
        });

        this.action_forward = new Ext.Action({
            text: this.app.i18n._('Forward'),
            handler: this.onMessageForward.createDelegate(this),
            iconCls: 'action_email_forward'
        });

        this.action_download = new Ext.Action({
            text: this.app.i18n._('Save'),
            handler: this.onMessageDownload.createDelegate(this),
            iconCls: 'action_email_download',
            disabled: this.record.id.match(/_/)
        });
        
        this.action_print = new Ext.Action({
            text: this.app.i18n._('Print Message'),
            handler: this.onMessagePrint.createDelegate(this.app.getMainScreen().getCenterPanel(), [this]),
            iconCls:'action_print',
            menu:{
                items:[
                    new Ext.Action({
                        text: this.app.i18n._('Print Preview'),
                        handler: this.onMessagePrintPreview.createDelegate(this.app.getMainScreen().getCenterPanel(), [this]),
                        iconCls:'action_printPreview'
                    })
                ]
            }
        });
        
    },
    
    /**
     * init toolbar
     */
    initToolbar: function() {
        // use toolbar from gridPanel
        this.tbar = new Ext.Toolbar({
            defaults: {height: 55},
            items: [{
                xtype: 'buttongroup',
                columns: 5,
                items: [
                    Ext.apply(new Ext.Button(this.action_deleteRecord), {
                        scale: 'medium',
                        rowspan: 2,
                        iconAlign: 'top'
                    }),
                    Ext.apply(new Ext.Button(this.action_reply), {
                        scale: 'medium',
                        rowspan: 2,
                        iconAlign: 'top'
                    }),
                    Ext.apply(new Ext.Button(this.action_replyAll), {
                        scale: 'medium',
                        rowspan: 2,
                        iconAlign: 'top'
                    }),
                    Ext.apply(new Ext.Button(this.action_forward), {
                        scale: 'medium',
                        rowspan: 2,
                        iconAlign: 'top'
                    }), 
                    Ext.apply(new Ext.SplitButton(this.action_print), {
                        scale: 'medium',
                        rowspan: 2,
                        iconAlign:'top',
                        arrowAlign:'right'
                    }), 
                    Ext.apply(new Ext.Button(this.action_download), {
                        scale: 'medium',
                        rowspan: 2,
                        iconAlign:'top',
                        arrowAlign:'right'
                    })
                ]
            }]
        });
        
    },
    
    /**
     * after render
     */
    afterRender: function() {
        this.supr().afterRender.apply(this, arguments);
        this.showMessage();

        var title = this.record.get('subject');
        if (title !== undefined) {
            // TODO make this work for attachment mails
            this.window.setTitle(title);
        }
    },
    
    /**
     * show message
     */
    showMessage: function() {
        this.layout.setActiveItem(this.getSingleRecordPanel());
        this.updateDetails(this.record, this.getSingleRecordPanel().body);
    },
    
    /**
     * executed after a msg compose
     * 
     * @param {String} composedMsg
     * @param {String} action
     * @param {Array}  [affectedMsgs]  messages affected 
     * 
     */
    onAfterCompose: function(composedMsg, action, affectedMsgs) {
        this.fireEvent('update', composedMsg, action, affectedMsgs);
    },
    
    /**
     * executed after deletion of this message
     */
    onAfterDelete: function() {
        this.fireEvent('remove', Ext.encode(this.record.data));
        this.window.close();
    },
    
    /**
     * download message
     */
    onMessageDownload: function() {
        var downloader = new Ext.ux.file.Download({
            params: {
                method: 'Felamimail.downloadMessage',
                requestType: 'HTTP',
                messageId: this.record.id
            }
        }).start();
    },
    
    /**
     * delete message handler
     */
    onMessageDelete: function(force) {
        var mainApp = Ext.ux.PopupWindowMgr.getMainWindow().Tine.Tinebase.appMgr.get('Felamimail'),
            folderId = this.record.get('folder_id'),
            folder = mainApp.getFolderStore().getById(folderId),
            accountId = folder ? folder.get('account_id') : null,
            account = mainApp.getAccountStore().getById(accountId),
            trashId = account ? account.getTrashFolderId() : null;
            
        this.loadMask.show();
        if (trashId) {
            var filter = [{field: 'id', operator: 'equals', value: this.record.id}];
            
            Tine.Felamimail.messageBackend.moveMessages(filter, trashId, {
                callback: this.onAfterDelete.createDelegate(this, ['move'])
            });
        } else {
            Tine.Felamimail.messageBackend.addFlags(this.record.id, '\\Deleted', {
                callback: this.onAfterDelete.createDelegate(this, ['flag'])
            });
        }
    },
    
    /**
     * reply message handler
     */
    onMessageReplyTo: function(toAll) {
        Tine.Felamimail.MessageEditDialog.openWindow({
            replyTo : Ext.encode(this.record.data),
            replyToAll: toAll,
            listeners: {
                'update': this.onAfterCompose.createDelegate(this, ['reply', Ext.encode([this.record.data])], 1)
            }
        });
    },
    
    /**
     * forward message handler
     */
    onMessageForward: function() {
        Tine.Felamimail.MessageEditDialog.openWindow({
            forwardMsgs : Ext.encode([this.record.data]),
            listeners: {
                'update': this.onAfterCompose.createDelegate(this, ['forward', Ext.encode([this.record.data])], 1)
            }
        });
    },
    
    onMessagePrint: Tine.Felamimail.GridPanel.prototype.onPrint,
    onMessagePrintPreview: Tine.Felamimail.GridPanel.prototype.onPrintPreview
});

Tine.Felamimail.MessageDisplayDialog.openWindow = function (config) {
    var id = (config.record && config.record.id) ? config.record.id : 0;
    var window = Tine.WindowFactory.getWindow({
        width: 800,
        height: 700,
        name: 'TineFelamimailMessageDisplayDialog_' + id,
        contentPanelConstructor: 'Tine.Felamimail.MessageDisplayDialog',
        contentPanelConstructorConfig: config
    });
    return window;
};
/*
 * Tine 2.0
 * 
 * @package     Felamimail
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2009-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 */
 
Ext.namespace('Tine.Felamimail');

/**
 * @namespace   Tine.Felamimail
 * @class       Tine.Felamimail.MessageEditDialog
 * @extends     Tine.widgets.dialog.EditDialog
 * 
 * <p>Message Compose Dialog</p>
 * <p>This dialog is for composing emails with recipients, body and attachments. 
 * you can choose from which account you want to send the mail.</p>
 * <p>
 * TODO         make email note editable
 * </p>
 * 
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * 
 * @param       {Object} config
 * @constructor
 * Create a new MessageEditDialog
 */
 Tine.Felamimail.MessageEditDialog = Ext.extend(Tine.widgets.dialog.EditDialog, {
    /**
     * @cfg {Array/String} bcc
     * initial config for bcc
     */
    bcc: null,
    
    /**
     * @cfg {String} body
     */
    msgBody: '',
    
    /**
     * @cfg {Array/String} cc
     * initial config for cc
     */
    cc: null,
    
    /**
     * @cfg {Array} of Tine.Felamimail.Model.Message (optionally encoded)
     * messages to forward
     */
    forwardMsgs: null,
    
    /**
     * @cfg {String} accountId
     * the accout id this message is sent from
     */
    accountId: null,
    
    /**
     * @cfg {Tine.Felamimail.Model.Message} (optionally encoded)
     * message to reply to
     */
    replyTo: null,

    /**
     * @cfg {Tine.Felamimail.Model.Message} (optionally encoded)
     * message to use as draft/template
     */
    draftOrTemplate: null,
    
    /**
     * @cfg {Boolean} (defaults to false)
     */
    replyToAll: false,
    
    /**
     * @cfg {String} subject
     */
    subject: '',
    
    /**
     * @cfg {Array/String} to
     * initial config for to
     */
    to: null,
    
    /**
     * validation error message
     * @type String
     */
    validationErrorMessage: '',
    
    /**
     * array with e-mail-addresses used as recipients
     * @type {Array}
     */
    mailAddresses: null,
    /**
     * json-encoded selection filter
     * @type {String} selectionFilter
     */
    selectionFilter: null,
    
    /**
     * holds default values for the record
     * @type {Object}
     */
    recordDefaults: null,
    
    /**
     * @private
     */
    windowNamePrefix: 'MessageEditWindow_',
    appName: 'Felamimail',
    recordClass: Tine.Felamimail.Model.Message,
    recordProxy: Tine.Felamimail.messageBackend,
    loadRecord: false,
    evalGrants: false,
    
    bodyStyle:'padding:0px',
    
    /**
     * overwrite update toolbars function (we don't have record grants)
     * @private
     */
    updateToolbars: Ext.emptyFn,
    
    //private
    initComponent: function() {
         Tine.Felamimail.MessageEditDialog.superclass.initComponent.call(this);
         this.on('save', this.onSave, this);
    },
    
    /**
     * init buttons
     */
    initButtons: function() {
        this.fbar = [];
        
        this.action_send = new Ext.Action({
            text: this.app.i18n._('Send'),
            handler: this.onSaveAndClose,
            iconCls: 'FelamimailIconCls',
            disabled: false,
            scope: this
        });

        this.action_searchContacts = new Ext.Action({
            text: this.app.i18n._('Search Recipients'),
            handler: this.onSearchContacts,
            iconCls: 'AddressbookIconCls',
            disabled: false,
            scope: this
        });
        
        this.action_saveAsDraft = new Ext.Action({
            text: this.app.i18n._('Save As Draft'),
            handler: this.onSaveInFolder.createDelegate(this, ['drafts_folder']),
            iconCls: 'action_saveAsDraft',
            disabled: false,
            scope: this
        });

        this.action_saveAsTemplate = new Ext.Action({
            text: this.app.i18n._('Save As Template'),
            handler: this.onSaveInFolder.createDelegate(this, ['templates_folder']),
            iconCls: 'action_saveAsTemplate',
            disabled: false,
            scope: this
        });
        
        // TODO think about changing icon onToggle
        this.action_saveEmailNote = new Ext.Action({
            text: this.app.i18n._('Save Email Note'),
            handler: this.onToggleSaveNote,
            iconCls: 'notes_noteIcon',
            disabled: false,
            scope: this,
            enableToggle: true
        });
        this.button_saveEmailNote = Ext.apply(new Ext.Button(this.action_saveEmailNote), {
            tooltip: this.app.i18n._('Activate this toggle button to save the email text as a note attached to the recipient(s) contact(s).')
        });

        this.tbar = new Ext.Toolbar({
            defaults: {height: 55},
            items: [{
                xtype: 'buttongroup',
                columns: 5,
                items: [
                    Ext.apply(new Ext.Button(this.action_send), {
                        scale: 'medium',
                        rowspan: 2,
                        iconAlign: 'top'
                    }),
                    Ext.apply(new Ext.Button(this.action_searchContacts), {
                        scale: 'medium',
                        rowspan: 2,
                        iconAlign: 'top',
                        tooltip: this.app.i18n._('Click to search for and add recipients from the Addressbook.')
                    }),
                    this.action_saveAsDraft,
                    this.button_saveEmailNote,
                    this.action_saveAsTemplate
                ]
            }]
        });
    },

    
    /**
     * @private
     */
    initRecord: function() {
        this.decodeMsgs();
        
        this.recordDefaults = Tine.Felamimail.Model.Message.getDefaultData();
        
        if (this.mailAddresses) {
            this.recordDefaults.to = Ext.decode(this.mailAddresses);
        } else if (this.selectionFilter) {
            this.on('load', this.fetchRecordsOnLoad, this);
        }
        
        if (! this.record) {
            this.record = new Tine.Felamimail.Model.Message(this.recordDefaults, 0);
        }
        this.initFrom();
        this.initRecipients();
        this.initSubject();
        this.initContent();
        // legacy handling:...
        // TODO add this information to attachment(s) + flags and remove this
        if (this.replyTo) {
            this.record.set('flags', '\\Answered');
            this.record.set('original_id', this.replyTo.id);
        } else if (this.forwardMsgs) {
            this.record.set('flags', 'Passed');
            this.record.set('original_id', this.forwardMsgs[0].id);
        } else if (this.draftOrTemplate) {
            this.record.set('original_id', this.draftOrTemplate.id);
        }
        
        Tine.log.debug('Tine.Felamimail.MessageEditDialog::initRecord() -> record:');
        Tine.log.debug(this.record);
    },
    
    /**
     * show loadMask (loadRecord is false in this dialog)
     * @param {} ct
     * @param {} position
     */
    onRender : function(ct, position) {
        Tine.Felamimail.MessageEditDialog.superclass.onRender.call(this, ct, position);
        this.loadMask.show();
    },
    
    /**
     * init attachments when forwarding message
     * 
     * @param {Tine.Felamimail.Model.Message} message
     */
    initAttachements: function(message) {
        if (message.get('attachments').length > 0) {
            this.record.set('attachments', [{
                name: message.get('subject'),
                type: 'message/rfc822',
                size: message.get('size'),
                id: message.id
            }]);
        }
    },
    
    /**
     * inits body and attachments from reply/forward/template
     */
    initContent: function() {
        if (! this.record.get('body')) {
            var account = Tine.Tinebase.appMgr.get('Felamimail').getAccountStore().getById(this.record.get('account_id'));
            
            if (! this.msgBody) {
                var message = this.getMessageFromConfig();
                if (message) {
                    if (! message.bodyIsFetched()) {
                        // self callback when body needs to be fetched
                        return this.recordProxy.fetchBody(message, this.initContent.createDelegate(this));
                    }
                    
                    this.setMessageBody(message, account);
                    
                    if (this.isForwardedMessage() || this.draftOrTemplate) {
                        this.initAttachements(message);
                    }
                }
            } 
            this.addSignature(account);
            
            this.record.set('body', this.msgBody);
        }
        
        delete this.msgBody;
        this.onRecordLoad();
    },
    
    /**
     * set message body: converts newlines, adds quotes
     * 
     * @param {Tine.Felamimail.Model.Message} message
     * @param {Tine.Felamimail.Model.Account} account
     */
    setMessageBody: function(message, account) {
        this.msgBody = message.get('body');
        
        if (account.get('display_format') == 'plain' || (account.get('display_format') == 'content_type' && message.get('body_content_type') == 'text/plain')) {
            this.msgBody = Ext.util.Format.nl2br(this.msgBody);
        }
        
        if (this.replyTo) {
            this.setMessageBodyReply();
        } else if (this.isForwardedMessage()) {
            this.setMessageBodyForward();
        }
    },
    
    /**
     * set message body for reply message
     */
    setMessageBodyReply: function() {
        var date = (this.replyTo.get('sent')) 
            ? this.replyTo.get('sent') 
            : ((this.replyTo.get('received')) ? this.replyTo.get('received') : new Date());
        
        this.msgBody = String.format(this.app.i18n._('On {0}, {1} wrote'), 
            Tine.Tinebase.common.dateTimeRenderer(date), 
            Ext.util.Format.htmlEncode(this.replyTo.get('from_name'))
        ) + ':<br/>'
          + '<blockquote class="felamimail-body-blockquote">' + this.msgBody + '</blockquote><br/>';
    },
    
    /**
     * returns true if message is forwarded
     * 
     * @return {Boolean}
     */
    isForwardedMessage: function() {
        return (this.forwardMsgs && this.forwardMsgs.length === 1);
    },
    
    /**
     * set message body for forwarded message
     */
    setMessageBodyForward: function() {
        this.msgBody = '<br/>-----' + this.app.i18n._('Original message') + '-----<br/>'
            + Tine.Felamimail.GridPanel.prototype.formatHeaders(this.forwardMsgs[0].get('headers'), false, true) + '<br/><br/>'
            + this.msgBody + '<br/>';
    },
    
    /**
     * add signature to message
     * 
     * @param {Tine.Felamimail.Model.Account} account
     */
    addSignature: function(account) {
        if (this.draftOrTemplate || ! account) {
            return;
        }

        var signaturePosition = (account.get('signature_position')) ? account.get('signature_position') : 'below',
            signature = Tine.Felamimail.getSignature(this.record.get('account_id'));
        if (signaturePosition == 'below') {
            this.msgBody += signature;
        } else {
            this.msgBody = signature + '<br/><br/>' + this.msgBody;
        }
    },
    
    /**
     * inits / sets sender of message
     */
    initFrom: function() {
        if (! this.record.get('account_id')) {
            if (! this.accountId) {
                var message = this.getMessageFromConfig(),
                    folderId = message ? message.get('folder_id') : null, 
                    folder = folderId ? Tine.Tinebase.appMgr.get('Felamimail').getFolderStore().getById(folderId) : null,
                    accountId = folder ? folder.get('account_id') : null;
                    
                if (! accountId) {
                    var activeAccount = Tine.Tinebase.appMgr.get('Felamimail').getActiveAccount();
                    accountId = (activeAccount) ? activeAccount.id : null;
                }
                
                this.accountId = accountId;
            }
            
            this.record.set('account_id', this.accountId);
        }
        delete this.accountId;
    },
    
    /**
     * after render
     */
    afterRender: function() {
        Tine.Felamimail.MessageEditDialog.superclass.afterRender.apply(this, arguments);
        
        this.getEl().on(Ext.EventManager.useKeydown ? 'keydown' : 'keypress', this.onKeyPress, this);
        this.recipientGrid.on('specialkey', function(field, e) {
            this.onKeyPress(e);
        }, this);
        
        this.recipientGrid.on('blur', function(editor) {
            // do not let the blur event reach the editor grid if we want the subjectField to have focus
            if (this.subjectField.hasFocus) {
                return false;
            }
        }, this);
        
        this.htmlEditor.on('keydown', function(e) {
            if (e.getKey() == e.ENTER && e.ctrlKey) {
                this.onSaveAndClose();
            } else if (e.getKey() == e.TAB && e.shiftKey) {
                this.subjectField.focus.defer(50, this.subjectField);
            }
        }, this);
    },
    
    /**
     * on key press
     * @param {} e
     * @param {} t
     * @param {} o
     */
    onKeyPress: function(e, t, o) {
        if ((e.getKey() == e.TAB || e.getKey() == e.ENTER) && ! e.shiftKey) {
            if (e.getTarget('input[name=subject]')) {
                this.htmlEditor.focus.defer(50, this.htmlEditor);
            } else if (e.getTarget('input[type=text]')) {
                this.subjectField.focus.defer(50, this.subjectField);
            }
        }
    },
    
    /**
     * returns message passed with config
     * 
     * @return {Tine.Felamimail.Model.Message}
     */
    getMessageFromConfig: function() {
        return this.replyTo ? this.replyTo : 
               this.forwardMsgs && this.forwardMsgs.length === 1 ? this.forwardMsgs[0] :
               this.draftOrTemplate ? this.draftOrTemplate : null;
    },
    
    /**
     * inits to/cc/bcc
     */
    initRecipients: function() {
        if (this.replyTo) {
            this.initReplyRecipients();
        }
        
        Ext.each(['to', 'cc', 'bcc'], function(field) {
            if (this.draftOrTemplate) {
                this[field] = this.draftOrTemplate.get(field);
            }
            
            if (! this.record.get(field)) {
                this[field] = Ext.isArray(this[field]) ? this[field] : Ext.isString(this[field]) ? [this[field]] : [];
                this.record.set(field, Ext.unique(this[field]));
            }
            delete this[field];
            
            this.resolveRecipientFilter(field);
            
        }, this);
    },
    
    /**
     * init recipients from reply/replyToAll information
     */
    initReplyRecipients: function() {
        var replyTo = this.replyTo.get('headers')['reply-to'];
        
        if (replyTo) {
            this.to = replyTo;
        } else {
            var toemail = '<' + this.replyTo.get('from_email') + '>';
            if (this.replyTo.get('from_name') && this.replyTo.get('from_name') != this.replyTo.get('from_email')) {
                this.to = this.replyTo.get('from_name') + ' ' + toemail;
            } else {
                this.to = toemail;
            }
        }
        
        if (this.replyToAll) {
            if (! Ext.isArray(this.to)) {
                this.to = [this.to];
            }
            this.to = this.to.concat(this.replyTo.get('to'));
            this.cc = this.replyTo.get('cc');
            
            // remove own email and all non-email strings/objects from to/cc
            var account = Tine.Tinebase.appMgr.get('Felamimail').getAccountStore().getById(this.record.get('account_id')),
                ownEmailRegexp = new RegExp(account.get('email'));
            Ext.each(['to', 'cc'], function(field) {
                for (var i=0; i < this[field].length; i++) {
                    if (! Ext.isString(this[field][i]) || ! this[field][i].match(/@/) || ownEmailRegexp.test(this[field][i])) {
                        this[field].splice(i, 1);
                    }
                }
            }, this);
        }
    },
    
    /**
     * resolve recipient filter / queries addressbook
     * 
     * @param {String} field to/cc/bcc
     */
    resolveRecipientFilter: function(field) {
        if (! Ext.isEmpty(this.record.get(field)) && Ext.isObject(this.record.get(field)[0]) &&  this.record.get(field)[0].operator) {
            // found a filter
            var filter = this.record.get(field);
            this.record.set(field, []);
            
            this['AddressLoadMask'] = new Ext.LoadMask(Ext.getBody(), {msg: this.app.i18n._('Loading Mail Addresses')});
            this['AddressLoadMask'].show();
            
            Tine.Addressbook.searchContacts(filter, null, function(response) {
                var mailAddresses = Tine.Felamimail.GridPanelHook.prototype.getMailAddresses(response.results);
                
                this.record.set(field, mailAddresses);
                this.recipientGrid.syncRecipientsToStore([field], this.record, true, false);
                this['AddressLoadMask'].hide();
                
            }.createDelegate(this));
        }
    },
    
    /**
     * sets / inits subject
     */
    initSubject: function() {
        if (! this.record.get('subject')) {
            if (! this.subject) {
                if (this.replyTo) {
                    this.setReplySubject();
                } else if (this.forwardMsgs) {
                    this.setForwardSubject();
                } else if (this.draftOrTemplate) {
                    this.subject = this.draftOrTemplate.get('subject');
                }
            }
            this.record.set('subject', this.subject);
        }
        
        delete this.subject;
    },
    
    /**
     * setReplySubject -> this.subject
     * 
     * removes existing prefixes + just adds 'Re: '
     */
    setReplySubject: function() {
        var replyPrefix = 'Re: ',
            replySubject = (this.replyTo.get('subject')) ? this.replyTo.get('subject') : '',
            replySubject = replySubject.replace(/^((re|aw|antw|fwd|odp|sv|wg|tr):\s*)*/i, replyPrefix);
            
        this.subject = replySubject;
    },
    
    /**
     * setForwardSubject -> this.subject
     */
    setForwardSubject: function() {
        this.subject =  this.app.i18n._('Fwd:') + ' ';
        this.subject += this.forwardMsgs.length === 1 ?
            this.forwardMsgs[0].get('subject') :
            String.format(this.app.i18n._('{0} Message', '{0} Messages', this.forwardMsgs.length));
    },
    
    /**
     * decode this.replyTo / this.forwardMsgs from interwindow json transport
     */
    decodeMsgs: function() {
        if (Ext.isString(this.draftOrTemplate)) {
            this.draftOrTemplate = new this.recordClass(Ext.decode(this.draftOrTemplate));
        }
        
        if (Ext.isString(this.replyTo)) {
            this.replyTo = new this.recordClass(Ext.decode(this.replyTo));
        }
        
        if (Ext.isString(this.forwardMsgs)) {
            var msgs = [];
            Ext.each(Ext.decode(this.forwardMsgs), function(msg) {
                msgs.push(new this.recordClass(msg));
            }, this);
            
            this.forwardMsgs = msgs;
        }
    },
    
    /**
     * fix input fields layout
     */
    fixLayout: function() {
        
        if (! this.subjectField.rendered || ! this.accountCombo.rendered || ! this.recipientGrid.rendered) {
            return;
        }
        
        var scrollWidth = this.recipientGrid.getView().getScrollOffset();
        this.subjectField.setWidth(this.subjectField.getWidth() - scrollWidth + 1);
        this.accountCombo.setWidth(this.accountCombo.getWidth() - scrollWidth + 1);
    },
    
    /**
     * save message in folder
     * 
     * @param {String} folderField
     */
    onSaveInFolder: function (folderField) {
        this.onRecordUpdate();
        
        var account = Tine.Tinebase.appMgr.get('Felamimail').getAccountStore().getById(this.record.get('account_id')),
            folderName = account.get(folderField);
                    
        if (! folderName || folderName == '') {
            Ext.MessageBox.alert(
                this.app.i18n._('Failed'), 
                String.format(this.app.i18n._('{0} account setting empty.'), folderField)
            );
        } else if (this.isValid()) {
            this.loadMask.show();
            this.recordProxy.saveInFolder(this.record, folderName, {
                scope: this,
                success: function(record) {
                    this.fireEvent('update', Ext.util.JSON.encode(this.record.data));
                    this.purgeListeners();
                    this.window.close();
                },
                failure: this.onRequestFailed,
                timeout: 150000 // 3 minutes
            });
        } else {
            Ext.MessageBox.alert(_('Errors'), _('Please fix the errors noted.'));
        }
    },
    
    /**
     * toggle save note
     * 
     * @param {} button
     * @param {} e
     */
    onToggleSaveNote: function (button, e) {
        this.record.set('note', (! this.record.get('note')));
    },
    
    /**
     * search for contacts as recipients
     */
    onSearchContacts: function() {
        Tine.Felamimail.RecipientPickerDialog.openWindow({
            record: new this.recordClass(Ext.copyTo({}, this.record.data, ['subject', 'to', 'cc', 'bcc']), Ext.id()),
            listeners: {
                scope: this,
                'update': function(record) {
                    var messageWithRecipients = Ext.isString(record) ? new this.recordClass(Ext.decode(record)) : record;
                    this.recipientGrid.syncRecipientsToStore(['to', 'cc', 'bcc'], messageWithRecipients, true, true);
                }
            }
        });
    },
    
    /**
     * executed after record got updated from proxy
     * 
     * @private
     */
    onRecordLoad: function() {
        // interrupt process flow till dialog is rendered
        if (! this.rendered) {
            this.onRecordLoad.defer(250, this);
            return;
        }
        
        var title = this.app.i18n._('Compose email:');
        if (this.record.get('subject')) {
            title = title + ' ' + this.record.get('subject');
        }
        this.window.setTitle(title);
        
        this.getForm().loadRecord(this.record);
        this.attachmentGrid.loadRecord(this.record);
        
        if (this.record.get('note') && this.record.get('note') == '1') {
            this.button_saveEmailNote.toggle();
        }
        var ticketFn = this.onAfterRecordLoad.deferByTickets(this),
            wrapTicket = ticketFn();
        
        this.fireEvent('load', this, this.record, ticketFn);
        wrapTicket();
    },

    /**
     * overwrite, just hide the loadMask
     */
    onAfterRecordLoad: function() {
        if(this.loadMask) {
            this.loadMask.hide();
        }
    },
        
    /**
     * executed when record gets updated from form
     * - add attachments to record here
     * - add alias / from
     * 
     * @private
     */
    onRecordUpdate: function() {
        this.record.data.attachments = [];
        var attachmentData = null;
        
        this.attachmentGrid.store.each(function(attachment) {
            this.record.data.attachments.push(Ext.ux.file.Upload.file.getFileData(attachment));
        }, this);
        
        var accountId = this.accountCombo.getValue(),
            account = this.accountCombo.getStore().getById(accountId),
            emailFrom = account.get('email');
            
        this.record.set('from_email', emailFrom);
        
        Tine.Felamimail.MessageEditDialog.superclass.onRecordUpdate.call(this);

        this.record.set('account_id', account.get('original_id'));
        
        // need to sync once again to make sure we have the correct recipients
        this.recipientGrid.syncRecipientsToRecord();
    },
    
    /**
     * show error if request fails
     * 
     * @param {} response
     * @param {} request
     * @private
     * 
     * TODO mark field(s) invalid if for example email is incorrect
     * TODO add exception dialog on critical errors?
     */
    onRequestFailed: function(response, request) {
        Ext.MessageBox.alert(
            this.app.i18n._('Failed'), 
            String.format(this.app.i18n._('Could not send {0}.'), this.i18nRecordName) 
                + ' ( ' + this.app.i18n._('Error:') + ' ' + response.message + ')'
        );
        this.saving = false;
        this.loadMask.hide();
    },

    /**
     * init attachment grid + add button to toolbar
     */
    initAttachmentGrid: function() {
        if (! this.attachmentGrid) {
        
            this.attachmentGrid = new Tine.widgets.grid.FileUploadGrid({
                fieldLabel: this.app.i18n._('Attachments'),
                hideLabel: true,
                filesProperty: 'attachments',
                // TODO     think about that -> when we deactivate the top toolbar, we lose the dropzone for files!
                //showTopToolbar: false,
                anchor: '100% 95%'
            });
            
            // add file upload button to toolbar
            this.action_addAttachment = this.attachmentGrid.getAddAction();
            this.action_addAttachment.plugins[0].dropElSelector = null;
            this.action_addAttachment.plugins[0].onBrowseButtonClick = function() {
                this.southPanel.expand();
            }.createDelegate(this);
            
            this.tbar.get(0).insert(1, Ext.apply(new Ext.Button(this.action_addAttachment), {
                scale: 'medium',
                rowspan: 2,
                iconAlign: 'top'
            }));
        }
    },
    
    /**
     * init account (from) combobox
     * 
     * - need to create a new store with an account record for each alias
     */
    initAccountCombo: function() {
        var accountStore = Tine.Tinebase.appMgr.get('Felamimail').getAccountStore(),
            accountComboStore = new Ext.data.ArrayStore({
                fields   : Tine.Felamimail.Model.Account
            });
        
        var aliasAccount = null,
            aliases = null,
            id = null
            
        accountStore.each(function(account) {
            aliases = [ account.get('email') ];

            if (account.get('type') == 'system') {
                // add identities / aliases to store (for systemaccounts)
                var user = Tine.Tinebase.registry.get('currentAccount');
                if (user.emailUser && user.emailUser.emailAliases && user.emailUser.emailAliases.length > 0) {
                    aliases = aliases.concat(user.emailUser.emailAliases);
                }
            }
            
            for (var i = 0; i < aliases.length; i++) {
                id = (i == 0) ? account.id : Ext.id();
                aliasAccount = account.copy(id);
                if (i > 0) {
                    aliasAccount.data.id = id;
                    aliasAccount.set('email', aliases[i]);
                }
                aliasAccount.set('name', aliasAccount.get('name') + ' (' + aliases[i] +')');
                aliasAccount.set('original_id', account.id);
                accountComboStore.add(aliasAccount);
            }
        }, this);
        
        this.accountCombo = new Ext.form.ComboBox({
            name: 'account_id',
            ref: '../../accountCombo',
            plugins: [ Ext.ux.FieldLabeler ],
            fieldLabel: this.app.i18n._('From'),
            displayField: 'name',
            valueField: 'id',
            editable: false,
            triggerAction: 'all',
            store: accountComboStore,
            mode: 'local',
            listeners: {
                scope: this,
                select: this.onFromSelect
            }
        });
    },
    
    /**
     * if 'account_id' is changed we need to update the signature
     * 
     * @param {} combo
     * @param {} newValue
     * @param {} oldValue
     */
     onFromSelect: function(combo, record, index) {
        
        // get new signature
        var accountId = record.get('original_id');
        var newSignature = Tine.Felamimail.getSignature(accountId);
        var signatureRegexp = new RegExp('<br><br><span id="felamimail\-body\-signature">\-\-<br>.*</span>');
        
        // update signature
        var bodyContent = this.htmlEditor.getValue();
        bodyContent = bodyContent.replace(signatureRegexp, newSignature);
        
        this.htmlEditor.setValue(bodyContent);
    },
    
    /**
     * returns dialog
     * 
     * NOTE: when this method gets called, all initialisation is done.
     * 
     * @return {Object}
     * @private
     */
    getFormItems: function() {
        
        this.initAttachmentGrid();
        this.initAccountCombo();
        
        this.recipientGrid = new Tine.Felamimail.RecipientGrid({
            record: this.record,
            i18n: this.app.i18n,
            hideLabel: true,
            composeDlg: this,
            autoStartEditing: !this.AddressLoadMask
        });
        
        this.southPanel = new Ext.Panel({
            region: 'south',
            layout: 'form',
            height: 150,
            split: true,
            collapseMode: 'mini',
            header: false,
            collapsible: true,
            collapsed: (this.record.bodyIsFetched() && (! this.record.get('attachments') || this.record.get('attachments').length == 0)),
            items: [this.attachmentGrid]
        });

        this.htmlEditor = new Tine.Felamimail.ComposeEditor({
            fieldLabel: this.app.i18n._('Body'),
            flex: 1  // Take up all *remaining* vertical space
        });
        
        return {
            border: false,
            frame: true,
            layout: 'border',
            items: [
                {
                region: 'center',
                layout: {
                    align: 'stretch',  // Child items are stretched to full width
                    type: 'vbox'
                },
                listeners: {
                    'afterlayout': this.fixLayout,
                    scope: this
                },
                items: [
                    this.accountCombo, 
                    this.recipientGrid, 
                {
                    xtype:'textfield',
                    plugins: [ Ext.ux.FieldLabeler ],
                    fieldLabel: this.app.i18n._('Subject'),
                    name: 'subject',
                    ref: '../../subjectField',
                    enableKeyEvents: true,
                    listeners: {
                        scope: this,
                        // update title on keyup event
                        'keyup': function(field, e) {
                            if (! e.isSpecialKey()) {
                                this.window.setTitle(
                                    this.app.i18n._('Compose email:') + ' ' 
                                    + field.getValue()
                                );
                            }
                        }
                    }
                }, this.htmlEditor
                ]
            }, this.southPanel]
        };
    },

    /**
     * is form valid (checks if attachments are still uploading / recipients set)
     * 
     * @return {Boolean}
     */
    isValid: function() {
        this.validationErrorMessage = Tine.Felamimail.MessageEditDialog.superclass.getValidationErrorMessage.call(this);
        
        var result = true;
        
        if (this.attachmentGrid.isUploading()) {
            result = false;
            this.validationErrorMessage = this.app.i18n._('Files are still uploading.');
        }
        
        if (result) {
            result = this.validateRecipients();
        }
        
        
        return (result && Tine.Felamimail.MessageEditDialog.superclass.isValid.call(this));
    },
    
    /**
     * generic apply changes handler
     * - NOTE: overwritten to check here if the subject is empty and if the user wants to send an empty message
     * 
     * @param {Ext.Button} button
     * @param {Event} event
     * @param {Boolean} closeWindow
     * 
     * TODO add note editing textfield here
     */
    onApplyChanges: function(closeWindow) {
        Tine.log.debug('Tine.Felamimail.MessageEditDialog::onApplyChanges');
        
        this.loadMask.show();
        
        if (this.getForm().findField('subject').getValue() == '') {
            Tine.log.debug('Tine.Felamimail.MessageEditDialog::onApplyChanges - empty subject');
            Ext.MessageBox.confirm(
                this.app.i18n._('Empty subject'),
                this.app.i18n._('Do you really want to send a message with an empty subject?'),
                function (button) {
                    Tine.log.debug('Tine.Felamimail.MessageEditDialog::doApplyChanges - button: ' + button);
                    if (button == 'yes') {
                        this.doApplyChanges(closeWindow);
                    }
                },
                this
            );
        } else {
            Tine.log.debug('Tine.Felamimail.MessageEditDialog::doApplyChanges - call parent');
            this.doApplyChanges(closeWindow);
        }
        
        /*
        if (this.record.data.note) {
            // show message box with note editing textfield
            //console.log(this.record.data.note);
            Ext.Msg.prompt(
                this.app.i18n._('Add Note'),
                this.app.i18n._('Edit Email Note Text:'), 
                function(btn, text) {
                    if (btn == 'ok'){
                        record.data.note = text;
                    }
                }, 
                this,
                100, // height of input area
                this.record.data.body 
            );
        }
        */
    },
    
    /**
     * checks recipients
     * 
     * @return {Boolean}
     */
    validateRecipients: function() {
        var result = true;
        
        if (this.record.get('to').length == 0 && this.record.get('cc').length == 0 && this.record.get('bcc').length == 0) {
            this.validationErrorMessage = this.app.i18n._('No recipients set.');
            result = false;
        }
        
        return result;
    },
    
    /**
     * get validation error message
     * 
     * @return {String}
     */
    getValidationErrorMessage: function() {
        return this.validationErrorMessage;
    },
    
    /**
     * fills the recipient grid with the records gotten from this.fetchRecordsOnLoad
     * @param {Array} contacts
     */
    fillRecipientGrid: function(contacts) {
        this.recipientGrid.addRecordsToStore(contacts, 'to');
        this.recipientGrid.setFixedHeight(true);
    },
    
    /**
     * fetches records to send an email to
     */
    fetchRecordsOnLoad: function(dialog, record, ticketFn) {
        var interceptor = ticketFn(),
            sf = Ext.decode(this.selectionFilter);
            
        Tine.log.debug('Fetching additional records...');
        Tine.Addressbook.contactBackend.searchRecords(sf, null, {
            scope: this,
            success: function(result) {
                this.fillRecipientGrid(result.records);
                interceptor();
            }
        });
        this.addressesLoaded = true;
    }
});

/**
 * Felamimail Edit Popup
 * 
 * @param   {Object} config
 * @return  {Ext.ux.Window}
 */
Tine.Felamimail.MessageEditDialog.openWindow = function (config) {
    var window = Tine.WindowFactory.getWindow({
        width: 700,
        height: 700,
        name: Tine.Felamimail.MessageEditDialog.prototype.windowNamePrefix + Ext.id(),
        contentPanelConstructor: 'Tine.Felamimail.MessageEditDialog',
        contentPanelConstructorConfig: config
    });
    return window;
};
/*
 * Tine 2.0
 * 
 * @package     Felamimail
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2009-2012 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */
 
Ext.namespace('Tine.Felamimail');

/**
 * @namespace   Tine.Felamimail
 * @class       Tine.Felamimail.AccountEditDialog
 * @extends     Tine.widgets.dialog.EditDialog
 * 
 * <p>Account Edit Dialog</p>
 * <p>
 * </p>
 * 
 * @author      Philipp Schuele <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2009 Metaways Infosystems GmbH (http://www.metaways.de)
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * 
 * @param       {Object} config
 * @constructor
 * Create a new Tine.Felamimail.AccountEditDialog
 * 
 */
Tine.Felamimail.AccountEditDialog = Ext.extend(Tine.widgets.dialog.EditDialog, {
    
    /**
     * @private
     */
    windowNamePrefix: 'AccountEditWindow_',
    appName: 'Felamimail',
    recordClass: Tine.Felamimail.Model.Account,
    recordProxy: Tine.Felamimail.accountBackend,
    loadRecord: false,
    tbarItems: [],
    evalGrants: false,
    
    /**
     * overwrite update toolbars function (we don't have record grants yet)
     * @private
     */
    updateToolbars: function() {

    },
    
    /**
     * executed after record got updated from proxy
     * 
     * -> only allow to change some of the fields if it is a system account
     */
    onRecordLoad: function() {
        Tine.Felamimail.AccountEditDialog.superclass.onRecordLoad.call(this);
        
        // if account type == system disable most of the input fields
        if (this.record.get('type') == 'system') {
            this.getForm().items.each(function(item) {
                // only enable some fields
                switch(item.name) {
                    case 'signature':
                    case 'signature_position':
                    case 'display_format':
                        break;
                    default:
                        item.setDisabled(true);
                }
            }, this);
        }
    },    
    
    /**
     * returns dialog
     * 
     * NOTE: when this method gets called, all initalisation is done.
     * @private
     */
    getFormItems: function() {

        this.signatureEditor = new Ext.form.HtmlEditor({
            fieldLabel: this.app.i18n._('Signature'),
            name: 'signature',
            autoHeight: true,
            getDocMarkup: function(){
                var markup = '<span id="felamimail\-body\-signature">'
                    + '</span>';
                return markup;
            },
            plugins: [
                new Ext.ux.form.HtmlEditor.RemoveFormat()
            ]
        });
        
        var commonFormDefaults = {
            xtype: 'textfield',
            anchor: '100%',
            labelSeparator: '',
            maxLength: 256,
            columnWidth: 1
        };
        
        return {
            xtype: 'tabpanel',
            deferredRender: false,
            border: false,
            activeTab: 0,
            items: [{
                title: this.app.i18n._('Account'),
                autoScroll: true,
                border: false,
                frame: true,
                xtype: 'columnform',
                formDefaults: commonFormDefaults,
                items: [[{
                    fieldLabel: this.app.i18n._('Account Name'),
                    name: 'name',
                    allowBlank: false
                }, {
                    fieldLabel: this.app.i18n._('User Email'),
                    name: 'email',
                    allowBlank: false,
                    vtype: 'email'
                }, {
                    fieldLabel: this.app.i18n._('User Name (From)'),
                    name: 'from'
                }, {
                    fieldLabel: this.app.i18n._('Organization'),
                    name: 'organization'
                }, this.signatureEditor,
                {
                    fieldLabel: this.app.i18n._('Signature position'),
                    name: 'signature_position',
                    typeAhead     : false,
                    triggerAction : 'all',
                    lazyRender    : true,
                    editable      : false,
                    mode          : 'local',
                    forceSelection: true,
                    value: 'below',
                    xtype: 'combo',
                    store: [
                        ['above', this.app.i18n._('Above the quote')],
                        ['below',  this.app.i18n._('Below the quote')]
                    ]
                }
                ]]
            }, {
                title: this.app.i18n._('IMAP'),
                autoScroll: true,
                border: false,
                frame: true,
                xtype: 'columnform',
                formDefaults: commonFormDefaults,
                items: [[{
                    fieldLabel: this.app.i18n._('Host'),
                    name: 'host',
                    allowBlank: false
                }, {
                    fieldLabel: this.app.i18n._('Port (Default: 143 / SSL: 993)'),
                    name: 'port',
                    allowBlank: false,
                    maxLength: 5,
                    xtype: 'numberfield'
                }, {
                    fieldLabel: this.app.i18n._('Secure Connection'),
                    name: 'ssl',
                    typeAhead     : false,
                    triggerAction : 'all',
                    lazyRender    : true,
                    editable      : false,
                    mode          : 'local',
                    forceSelection: true,
                    value: 'none',
                    xtype: 'combo',
                    store: [
                        ['none', this.app.i18n._('None')],
                        ['tls',  this.app.i18n._('TLS')],
                        ['ssl',  this.app.i18n._('SSL')]
                    ]
                },{
                    fieldLabel: this.app.i18n._('Username'),
                    name: 'user',
                    allowBlank: false
                }, {
                    fieldLabel: this.app.i18n._('Password'),
                    name: 'password',
                    emptyText: 'password',
                    inputType: 'password'
                }]]
            }, {
                title: this.app.i18n._('SMTP'),
                autoScroll: true,
                border: false,
                frame: true,
                xtype: 'columnform',
                formDefaults: commonFormDefaults,
                items: [[ {
                    fieldLabel: this.app.i18n._('Host'),
                    name: 'smtp_hostname'
                }, {
                    fieldLabel: this.app.i18n._('Port (Default: 25)'),
                    name: 'smtp_port',
                    maxLength: 5,
                    xtype:'numberfield',
                    allowBlank: false
                }, {
                    fieldLabel: this.app.i18n._('Secure Connection'),
                    name: 'smtp_ssl',
                    typeAhead     : false,
                    triggerAction : 'all',
                    lazyRender    : true,
                    editable      : false,
                    mode          : 'local',
                    value: 'none',
                    xtype: 'combo',
                    store: [
                        ['none', this.app.i18n._('None')],
                        ['tls',  this.app.i18n._('TLS')],
                        ['ssl',  this.app.i18n._('SSL')]
                    ]
                }, {
                    fieldLabel: this.app.i18n._('Authentication'),
                    name: 'smtp_auth',
                    typeAhead     : false,
                    triggerAction : 'all',
                    lazyRender    : true,
                    editable      : false,
                    mode          : 'local',
                    xtype: 'combo',
                    value: 'login',
                    store: [
                        ['none',    this.app.i18n._('None')],
                        ['login',   this.app.i18n._('Login')],
                        ['plain',   this.app.i18n._('Plain')]
                    ]
                },{
                    fieldLabel: this.app.i18n._('Username (optional)'),
                    name: 'smtp_user'
                }, {
                    fieldLabel: this.app.i18n._('Password (optional)'),
                    name: 'smtp_password',
                    emptyText: 'password',
                    inputType: 'password'
                }]]
            }, {
                title: this.app.i18n._('Sieve'),
                autoScroll: true,
                border: false,
                frame: true,
                xtype: 'columnform',
                formDefaults: commonFormDefaults,
                items: [[{
                    fieldLabel: this.app.i18n._('Host'),
                    name: 'sieve_hostname',
                    maxLength: 64
                }, {
                    fieldLabel: this.app.i18n._('Port (Default: 2000)'),
                    name: 'sieve_port',
                    maxLength: 5,
                    xtype:'numberfield'
                }, {
                    fieldLabel: this.app.i18n._('Secure Connection'),
                    name: 'sieve_ssl',
                    typeAhead     : false,
                    triggerAction : 'all',
                    lazyRender    : true,
                    editable      : false,
                    mode          : 'local',
                    value: 'none',
                    xtype: 'combo',
                    store: [
                        ['none', this.app.i18n._('None')],
                        ['tls',  this.app.i18n._('TLS')]
                    ]
                }]]
            }, {
                title: this.app.i18n._('Other Settings'),
                autoScroll: true,
                border: false,
                frame: true,
                xtype: 'columnform',
                formDefaults: commonFormDefaults,
                items: [[{
                    fieldLabel: this.app.i18n._('Sent Folder Name'),
                    name: 'sent_folder',
                    xtype: 'felamimailfolderselect',
                    account: this.record,
                    maxLength: 64
                }, {
                    fieldLabel: this.app.i18n._('Trash Folder Name'),
                    name: 'trash_folder',
                    xtype: 'felamimailfolderselect',
                    account: this.record,
                    maxLength: 64
                }, {
                    fieldLabel: this.app.i18n._('Drafts Folder Name'),
                    name: 'drafts_folder',
                    xtype: 'felamimailfolderselect',
                    account: this.record,
                    maxLength: 64
                }, {
                    fieldLabel: this.app.i18n._('Templates Folder Name'),
                    name: 'templates_folder',
                    xtype: 'felamimailfolderselect',
                    account: this.record,
                    maxLength: 64
                }, {
                    fieldLabel: this.app.i18n._('Display Format'),
                    name: 'display_format',
                    typeAhead     : false,
                    triggerAction : 'all',
                    lazyRender    : true,
                    editable      : false,
                    mode          : 'local',
                    forceSelection: true,
                    value: 'html',
                    xtype: 'combo',
                    store: [
                        ['html', this.app.i18n._('HTML')],
                        ['plain',  this.app.i18n._('Plain Text')],
                        ['content_type',  this.app.i18n._('Depending on content type (experimental)')]
                    ]
                }]]
            }]
        };
    },
    
    /**
     * generic request exception handler
     * 
     * @param {Object} exception
     */
    onRequestFailed: function(exception) {
        Tine.Felamimail.handleRequestException(exception);
        this.loadMask.hide();
    }    
});

/**
 * Felamimail Account Edit Popup
 * 
 * @param   {Object} config
 * @return  {Ext.ux.Window}
 */
 Tine.Felamimail.AccountEditDialog.openWindow = function (config) {
    var window = Tine.WindowFactory.getWindow({
        width: 580,
        height: 500,
        name: Tine.Felamimail.AccountEditDialog.prototype.windowNamePrefix + Ext.id(),
        contentPanelConstructor: 'Tine.Felamimail.AccountEditDialog',
        contentPanelConstructorConfig: config
    });
    return window;
};
/*
 * Tine 2.0
 * contacts combo box and store
 * 
 * @package     Addressbook
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2007-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */

Ext.ns('Tine.Addressbook');

/**
 * contact selection combo box
 * 
 * @namespace   Tine.Addressbook
 * @class       Tine.Addressbook.SearchCombo
 * @extends     Ext.form.ComboBox
 * 
 * <p>Contact Search Combobox</p>
 * <p><pre>
 * TODO         make this a twin trigger field with 'clear' button?
 * TODO         add switch to filter for expired/enabled/disabled user accounts
 * </pre></p>
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schuele <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2007-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 * 
 * @param       {Object} config
 * @constructor
 * Create a new Tine.Addressbook.SearchCombo
 * 
 * TODO         add     forceSelection: true ?
 */
Tine.Addressbook.SearchCombo = Ext.extend(Tine.Tinebase.widgets.form.RecordPickerComboBox, {
    
    /**
     * @cfg {Boolean} userOnly
     */
    userOnly: false,
    
    /**
     * @property additionalFilters
     * @type Array
     */
    additionalFilters: null,
    
    /**
     * use account objects/records in get/setValue
     * 
     * @cfg {Boolean} legacy
     * @legacy
     * 
     * TODO remove this later
     */
    useAccountRecord: false,
    allowBlank: false,
    
    itemSelector: 'div.search-item',
    minListWidth: 350,
    
    //private
    initComponent: function(){
        this.recordClass = Tine.Addressbook.Model.Contact;
        this.recordProxy = Tine.Addressbook.contactBackend;

        this.initTemplate();
        Tine.Addressbook.SearchCombo.superclass.initComponent.call(this);
    },
    
    /**
     * is called in accountMode to reset the value
     * @param value
     */
    processValue: function(value) {
        if (this.useAccountRecord) {
            if(value == '') {
                this.accountId = null;
                this.selectedRecord = null;
            }
        }
        return Tine.Addressbook.SearchCombo.superclass.processValue.call(this, value);
    },

    /**
     * use beforequery to set query filter
     * 
     * @param {Event} qevent
     */
    onBeforeQuery: function(qevent){
        Tine.Addressbook.SearchCombo.superclass.onBeforeQuery.apply(this, arguments);
        
        var filter = this.store.baseParams.filter;
        
        if (this.userOnly) {
            filter.push({field: 'type', operator: 'equals', value: 'user'});
        }
        
        if (this.additionalFilters !== null && this.additionalFilters.length > 0) {
            for (var i = 0; i < this.additionalFilters.length; i++) {
                filter.push(this.additionalFilters[i]);
            }
        }
    },
    
    /**
     * init template
     * @private
     */
    initTemplate: function() {
        // Custom rendering Template
        // TODO move style def to css ?
        if (! this.tpl) {
            this.tpl = new Ext.XTemplate(
                '<tpl for="."><div class="search-item">',
                    '<table cellspacing="0" cellpadding="2" border="0" style="font-size: 11px;" width="100%">',
                        '<tr>',
                            '<td width="30%"><b>{[this.encode(values.n_fileas)]}</b><br/>{[this.encode(values.org_name)]}</td>',
                            '<td width="25%">{[this.encode(values.adr_one_street)]}<br/>',
                                '{[this.encode(values.adr_one_postalcode)]} {[this.encode(values.adr_one_locality)]}</td>',
                            '<td width="25%">{[this.encode(values.tel_work)]}<br/>{[this.encode(values.tel_cell)]}</td>',
                            '<td width="20%">',
                                '<img width="45px" height="39px" src="{jpegphoto}" />',
                            '</td>',
                        '</tr>',
                    '</table>',
                '</div></tpl>',
                {
                    encode: function(value) {
                         if (value) {
                            return Ext.util.Format.htmlEncode(value);
                        } else {
                            return '';
                        }
                    }
                }
            );
        }
    },
    
    getValue: function() {
        if (this.useAccountRecord) {
            if (this.selectedRecord) {
                return this.selectedRecord.get('account_id');
            } else {
                return this.accountId;
            }
        } else {
            return Tine.Addressbook.SearchCombo.superclass.getValue.call(this);
        }
    },

    setValue: function (value) {

        if (this.useAccountRecord) {
            if (value) {
                if(value.accountId) {
                    // account object
                    this.accountId = value.accountId;
                    value = value.accountDisplayName;
                } else if (typeof(value.get) == 'function') {
                    // account record
                    this.accountId = value.get('id');
                    value = value.get('name');
                }
            } else {
                this.accountId = null;
                this.selectedRecord = null;
                
            }
        }
        return Tine.Addressbook.SearchCombo.superclass.setValue.call(this, value);
    }

});

Ext.reg('addressbookcontactpicker', Tine.Addressbook.SearchCombo);
Tine.widgets.form.RecordPickerManager.register('Addressbook', 'Contact', Tine.Addressbook.SearchCombo);
/*
 * Tine 2.0
 * 
 * @package     Felamimail
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2009-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */
 
Ext.namespace('Tine.Felamimail');

/**
 * @namespace   Tine.Felamimail
 * @class       Tine.Felamimail.ContactSearchCombo
 * @extends     Tine.Addressbook.SearchCombo
 * 
 * <p>Email Search ComboBox</p>
 * <p></p>
 * <pre></pre>
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * 
 * @param       {Object} config
 * @constructor
 * Create a new Tine.Felamimail.ContactSearchCombo
 */
Tine.Felamimail.ContactSearchCombo = Ext.extend(Tine.Addressbook.SearchCombo, {

    /**
     * @cfg {Boolean} forceSelection
     */
    forceSelection: false,
    
    /**
     * @private
     */
    initComponent: function() {
        // add additional filter to show only contacts with email addresses
        this.additionalFilters = [{field: 'email_query', operator: 'contains', value: '@' }];
        
        this.tpl = new Ext.XTemplate(
            '<tpl for="."><div class="search-item">',
                '{[this.encode(values.n_fileas)]}',
                ' (<b>{[this.encode(values.email, values.email_home)]}</b>)',
            '</div></tpl>',
            {
                encode: function(email, email_home) {
                    if (email) {
                        return Ext.util.Format.htmlEncode(email);
                    } else if (email_home) {
                        return Ext.util.Format.htmlEncode(email_home);
                    } else {
                        return '';
                    }
                }
            }
        );
        
        Tine.Felamimail.ContactSearchCombo.superclass.initComponent.call(this);
        
        this.store.on('load', this.onStoreLoad, this);
    },
    
    /**
     * override default onSelect
     * - set email/name as value
     * 
     * @param {} record
     * @private
     */
    onSelect: function(record) {
        var value = Tine.Felamimail.getEmailStringFromContact(record);
        this.setValue(value);
        
        this.collapse();
        this.fireEvent('blur', this);
    },
    
    /**
     * on load handler of combo store
     * -> add additional record if contact has multiple email addresses
     * 
     * @param {} store
     * @param {} records
     * @param {} options
     */
    onStoreLoad: function(store, records, options) {
        this.addAlternativeEmail(store, records);
        this.removeDuplicates(store);
    },
    
    /**
     * add alternative email addresses
     * 
     * @param {} store
     * @param {} records
     */
    addAlternativeEmail: function(store, records) {
        var index = 0,
            newRecord,
            recordData;
            
        Ext.each(records, function(record) {
            if (record.get('email') && record.get('email_home') && record.get('email') !== record.get('email_home')) {
                index++;
                recordData = Ext.copyTo({}, record.data, ['email_home', 'n_fileas']);
                newRecord = Tine.Addressbook.contactBackend.recordReader({responseText: Ext.util.JSON.encode(recordData)});
                newRecord.id = Ext.id();
                
                Tine.log.debug('add alternative: ' + Tine.Felamimail.getEmailStringFromContact(newRecord));
                store.insert(index, [newRecord]);
            }
            index++;
        });
    },
    
    /**
     * remove duplicate contacts
     * 
     * @param {} store
     */
    removeDuplicates: function(store) {
        var duplicates = null;
        
        store.each(function(record) {
            duplicates = store.queryBy(function(contact) {
                return record.id !== contact.id && Tine.Felamimail.getEmailStringFromContact(record) == Tine.Felamimail.getEmailStringFromContact(contact);
            });
            if (duplicates.getCount() > 0) {
                Tine.log.debug('remove duplicate: ' + Tine.Felamimail.getEmailStringFromContact(record));
                store.remove(record);
            }
        });
    }    
});
Ext.reg('felamimailcontactcombo', Tine.Felamimail.ContactSearchCombo);
/*
 * Tine 2.0
 * 
 * @package     Felamimail
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2009-2012 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */
 
Ext.namespace('Tine.Felamimail');

/**
 * @namespace   Tine.Felamimail
 * @class       Tine.Felamimail.RecipientGrid
 * @extends     Ext.grid.EditorGridPanel
 * 
 * <p>Recipient Grid Panel</p>
 * <p>grid panel for to/cc/bcc recipients</p>
 * <pre>
 * </pre>
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * 
 * @param       {Object} config
 * @constructor
 * Create a new Tine.Felamimail.RecipientGrid
 */
Tine.Felamimail.RecipientGrid = Ext.extend(Ext.grid.EditorGridPanel, {
    
    /**
     * @private
     */
    cls: 'felamimail-recipient-grid',
    
    /**
     * the message record
     * @type Tine.Felamimail.Model.Message
     * @property record
     */
    record: null,
    
    /**
     * message compose dlg
     * @type Tine.Felamimail.MessageEditDialog
     */
    composeDlg: null,
    
    /**
     * @type Ext.Menu
     * @property contextMenu
     */
    contextMenu: null,
    
    /**
     * @type Ext.data.SimpleStore
     * @property store
     */
    store: null,
    
    /**
     * @cfg {Boolean} autoStartEditing
     */
    autoStartEditing: true,
    
    /**
     * @cfg {String} autoExpandColumn
     * auto expand column of grid
     */
    autoExpandColumn: 'address',
    
    /**
     * @cfg {Number} clicksToEdit
     * clicks to edit for editor grid panel
     */
    clicksToEdit:1,
    
    /**
     * @cfg {Number} numberOfRecordsForFixedHeight
     */
    numberOfRecordsForFixedHeight: 6,

    /**
     * @cfg {Boolean} header
     * show header
     */
    header: false,
    
    /**
     * @cfg {Boolean} border
     * show border
     */
    border: false,
    
    /**
     * @cfg {Boolean} deferredRender
     * deferred rendering
     */
    deferredRender: false,
    
    forceValidation: true,
    
    enableDrop: true,
    ddGroup: 'recipientDDGroup',
    
    /**
     * @private
     */
    initComponent: function() {
        
        this.initStore();
        this.initColumnModel();
        this.initActions();
        this.sm = new Ext.grid.RowSelectionModel();
        
        Tine.Felamimail.RecipientGrid.superclass.initComponent.call(this);
        
        this.on('rowcontextmenu', this.onCtxMenu, this);
        // this is relayed by the contact search combo
        this.on('contextmenu', this.onCtxMenu.createDelegate(this, [this, null], 0), this);
            
        this.on('beforeedit', this.onBeforeEdit, this);
        this.on('afteredit', this.onAfterEdit, this);
    },
    
    /**
     * show context menu
     * 
     * @param {Tine.Felamimail.RecipientGrid} grid
     * @param {Number} row
     * @param {Event} e
     */
    onCtxMenu: function(grid, row, e) {
        var activeRow = (row === null) ? ((this.activeEditor) ? this.activeEditor.row : 0) : row;
        
        e.stopEvent();
        var selModel = grid.getSelectionModel();
        if (! selModel.isSelected(activeRow)) {
            selModel.selectRow(activeRow);
        }
        
        var record = this.store.getAt(activeRow);
        if (record) {
            this.action_remove.setDisabled(record.get('address') == '');
            this.contextMenu.showAt(e.getXY());
        }
    },
    
    /**
     * init store
     * @private
     */
    initStore: function() {
        
        if(!this.record) {
            this.initStore.defer(200, this);
            return false;
        }
        
        this.store = new Ext.data.SimpleStore({
            fields   : ['type', 'address']
        });
        
        // init recipients (on reply/reply to all)
        this.syncRecipientsToStore(['to', 'cc', 'bcc']);
        
        this.store.add(new Ext.data.Record({type: 'to', 'address': ''}));
        
        this.store.on('update', this.onUpdateStore, this);
        this.store.on('add', this.onAddStore, this);
    },
    
    /**
     * init cm
     * @private
     */
    initColumnModel: function() {
        
        var app = Tine.Tinebase.appMgr.get('Felamimail');
        
        this.searchCombo = new Tine.Felamimail.ContactSearchCombo({
            listeners: {
                scope: this,
                specialkey: this.onSearchComboSpecialkey,
                blur: function(combo) {
                    this.getView().el.select('.x-grid3-td-address-editing').removeClass('x-grid3-td-address-editing');
                    
                    // need to update record because we relay blur event and it might not be updated otherwise
                    if (this.activeEditor) {
                        var value = combo.getValue();
                        if (value !== null && this.activeEditor.record.get('address') != value) {
                            this.activeEditor.record.set('address', value);
                        }
                    }
                    this.stopEditing();
                }
            }
        });
        
        this.cm = new Ext.grid.ColumnModel([
            {
                resizable: true,
                id: 'type',
                dataIndex: 'type',
                width: 104,
                menuDisabled: true,
                header: 'type',
                renderer: function(value) {
                    var result = '',
                        qtip = Ext.util.Format.htmlEncode(app.i18n._('Click here to set To/CC/BCC.'));

                    switch(value) {
                        case 'to':
                            result = Ext.util.Format.htmlEncode(app.i18n._('To:'));
                            break;
                        case 'cc':
                            result = Ext.util.Format.htmlEncode(app.i18n._('Cc:'));
                            break;
                        case 'bcc':
                            result = Ext.util.Format.htmlEncode(app.i18n._('Bcc:'));
                            break;
                    }
                    
                    result = Tine.Tinebase.common.cellEditorHintRenderer(result);
                    
                    return '<div qtip="' + qtip +'">' + result + '</div>';
                },
                editor: new Ext.form.ComboBox({
                    typeAhead     : false,
                    triggerAction : 'all',
                    lazyRender    : true,
                    editable      : false,
                    mode          : 'local',
                    value         : null,
                    forceSelection: true,
                    lazyInit      : false,
                    store         : [
                        ['to',  app.i18n._('To:')],
                        ['cc',  app.i18n._('Cc:')],
                        ['bcc', app.i18n._('Bcc:')]
                    ],
                    listeners: {
                        focus: function(combo) {
                            combo.onTriggerClick();
                        }
                    }
                })
            },{
                resizable: true,
                menuDisabled: true,
                id: 'address',
                dataIndex: 'address',
                header: 'address',
                editor: this.searchCombo
            }
        ]);
    },
    
    /**
     * specialkey is pressed in search combo
     * 
     * @param {Combo} combo
     * @param {Event} e
     */
    onSearchComboSpecialkey: function(combo, e) {
        if (! this.activeEditor) {
            return;
        }
        
        var value = combo.getValue(),
            rawValue = combo.getRawValue();
        
        if (e.getKey() == e.ENTER) {
            // cancel loading when ENTER is pressed
            combo.lastStoreTransactionId = null;
            if (value !== '' || rawValue !== '') {
                // add another row here as this is not detected by onAfterEdit
                if (value !== rawValue) {
                    this.activeEditor.record.set('address', rawValue);
                    this.activeEditor.record.id = Ext.id();
                }
                this.addRowAndDoLayout(this.activeEditor.record);
                return true;
            }
        } else if (e.getKey() == e.BACKSPACE) {
            // remove row on backspace if we have more than 1 rows in grid
            if (rawValue == '' && this.store.getCount() > 1 && this.activeEditor.row > 0) {
                this.store.remove(this.activeEditor.record);
                this.activeEditor.row -= 1;
                this.setFixedHeight(false);
                this.ownerCt.doLayout();
                this.startEditing.defer(50, this, [this.activeEditor.row, this.activeEditor.col]);
                return true;
            }
        } else if (e.getKey() == e.ESC) {
            // TODO should ESC close the compose window if search combo is already empty?
//            if (value == '') {
//                this.fireEvent('specialkey', this, e);
//            }
            this.startEditing.defer(50, this, [this.activeEditor.row, this.activeEditor.col]);
            return true;
        }

        // jump to subject if we are in the last row and it is empty OR TAB was pressed
        if (e.getKey() == e.TAB || (value == '' && this.store.getCount() == this.activeEditor.row + 1)) {
            this.fireEvent('specialkey', combo, e);
            if (this.activeEditor.row == 0) {
                return false;
            } else {
                this.getView().el.select('.x-grid3-td-address-editing').removeClass('x-grid3-td-address-editing');
            }
        }
    },
    
    /**
     * adds row and adjusts layout
     * 
     * @param {} oldRecord
     */
    addRowAndDoLayout: function(oldRecord) {
        this.store.add(new Ext.data.Record({type: oldRecord.data.type, 'address': ''}));
        this.store.commitChanges();
        this.setFixedHeight(false);
        this.ownerCt.doLayout();
    },        
    
    /**
     * start editing (check if message compose dlg is saving/sending first)
     * 
     * @param {} row
     * @param {} col
     */
    startEditing: function(row, col) {
        if (! this.composeDlg || ! this.composeDlg.saving) {
            Tine.Felamimail.RecipientGrid.superclass.startEditing.apply(this, arguments);
        }
    },
    
    /**
     * init actions / ctx menu
     * @private
     */
    initActions: function() {
        this.action_remove = new Ext.Action({
            text: _('Remove'),
            handler: this.onDelete,
            iconCls: 'action_delete',
            scope: this
        });
        
        this.contextMenu = new Ext.menu.Menu({
            items:  this.action_remove
        });
    },
    
    /**
     * start editing after render
     * @private
     */
    afterRender: function() {
        Tine.Felamimail.RecipientGrid.superclass.afterRender.call(this);
        
        // kill x-scrollers
        this.el.child('div[class=x-grid3-scroller]').setStyle('overflow-x', 'hidden');
        
        if (this.autoStartEditing && this.store.getCount() == 1) {
            this.startEditing.defer(200, this, [0, 1]);
        }
        
        this.setFixedHeight(true);
        
        this.relayEvents(this.searchCombo, ['blur']);
        
        this.initDropTarget();
    },
    
    /**
     * init drop target with notifyDrop fn 
     * - adds new records from drag data to the recipient store
     */
    initDropTarget: function() {
        var dropTargetEl = this.getView().scroller.dom;
        var dropTarget = new Ext.dd.DropTarget(dropTargetEl, {
            ddGroup    : 'recipientDDGroup',
            notifyDrop : function(ddSource, e, data) {
                this.grid.addRecordsToStore(ddSource.dragData.selections);
                return true;
            },
            grid: this
        });
    },
    
    /**
     * add records to recipient store
     * 
     * @param {Array} records
     * @param {String} type
     */
    addRecordsToStore: function(records, type) {
        if (! type) {
            var emptyRecord = this.store.getAt(this.store.findExact('address', '')),
                type = (emptyRecord) ? emptyRecord.get('type') : 'to';
        }
                        
        var hasEmail = false,
            added = false;

        Ext.each(records, function(record) {
            if (record.hasEmail()) {
                this.store.add(new Ext.data.Record({type: type, 'address': Tine.Felamimail.getEmailStringFromContact(record)}));
                added = true;
            }
        }, this);
    },
    
    /**
     * set grid to fixed height if it has more than X records
     *  
     * @param {} doLayout
     */
    setFixedHeight: function (doLayout) {
        if (this.store.getCount() > this.numberOfRecordsForFixedHeight) {
            this.setHeight(155);
        } else {
            this.setHeight(this.store.getCount()*24 + 1);
        }

        if (doLayout && doLayout === true) {
            this.ownerCt.doLayout();
        }
    },
    
    /**
     * store has been updated
     * 
     * @param {} store
     * @param {} record
     * @param {} operation
     * @private
     */
    onUpdateStore: function(store, record, operation) {
        this.syncRecipientsToRecord();
    },
    
    /**
     * on add event of store
     * 
     * @param {} store
     * @param {} records
     * @param {} index
     */
    onAddStore: function(store, records, index) {
        this.syncRecipientsToRecord();
    },
    
    /**
     * sync grid with record
     * -> update record to/cc/bcc
     */
    syncRecipientsToRecord: function() {
        // update record recipient fields
        this.record.data.to = [];
        this.record.data.cc = [];
        this.record.data.bcc = [];
        this.store.each(function(recipient){
            if (recipient.data.address != '') {
                this.record.data[recipient.data.type].push(recipient.data.address);
            }
        }, this);
    },

    /**
     * sync grid with record
     * -> update store
     * 
     * @param {Array} fields
     * @param {Tine.Felamimail.Model.Message} record
     * @param {Boolean} setHeight
     * @param {Boolean} clearStore
     */
    syncRecipientsToStore: function(fields, record, setHeight, clearStore) {
        if (clearStore) {
            this.store.removeAll(true);
        }
        
        record = record || this.record;
        
        Ext.each(fields, function(field) {
            this._addRecipients(record.get(field), field);
        }, this);
        this.store.sort('address');
        
        if (clearStore) {
            this.store.add(new Ext.data.Record({type: 'to', 'address': ''}));
        }
        
        if (setHeight && setHeight === true) {
            this.setFixedHeight(true);
        }
    },
    
    /**
     * after edit
     * 
     * @param {} o
     */
    onAfterEdit: function(o) {
        if (o.field == 'address') {
            Ext.fly(this.getView().getCell(o.row, o.column)).removeClass('x-grid3-td-address-editing');
            if (o.value != '' && (o.originalValue == '' || this.store.findExact('address', '') === -1)) {
                // use selected type to create new row with empty address and start editing
                this.addRowAndDoLayout(o.record);
                this.startEditing.defer(50, this, [o.row +1, o.column]);
                this.searchCombo.focus.defer(80, this.searchCombo);
            }
        }
    },    
    
    /**
     * delete handler
     */
    onDelete: function(btn, e) {
        var sm = this.getSelectionModel();
        var records = sm.getSelections();
        Ext.each(records, function(record) {
            if (record.get('address') != '') {
                this.store.remove(record);
                this.store.fireEvent('update', this.store);
            }
        }, this);
        
        this.setFixedHeight(true);
    },
    
    /**
     * on before edit
     * 
     * @param {} o
     */
    onBeforeEdit: function(o) {
        this.getView().el.select('.x-grid3-td-address-editing').removeClass('x-grid3-td-address-editing');
        Ext.fly(this.getView().getCell(o.row, o.column)).addClass('x-grid3-td-address-editing');
    },
    
    
    /**
     * add recipients to grid store
     * 
     * @param {Array} recipients
     * @param {String} type
     * @private
     */
    _addRecipients: function(recipients, type) {
        if (recipients) {
            recipients = Ext.unique(recipients);
            for (var i=0; i < recipients.length; i++) {
                this.store.add(new Ext.data.Record({type: type, 'address': recipients[i]}));
            }
        }
    }
});

Ext.reg('felamimailrecipientgrid', Tine.Felamimail.RecipientGrid);
/*
 * Tine 2.0
 * 
 * @package     Felamimail
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2009-2012 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */
 
Ext.ns('Tine.Felamimail');

/**
 * @namespace   Tine.Felamimail
 * @class       Tine.Felamimail.Application
 * @extends     Tine.Tinebase.Application
 * 
 * <p>Felamimail application obj</p>
 * <p>
 * </p>
 * 
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * 
 * @param       {Object} config
 * 
 * @constructor
 * Create a new  Tine.Felamimail.Application
 */
Tine.Felamimail.Application = Ext.extend(Tine.Tinebase.Application, {
    
    /**
     * auto hook text _('New Mail')
     */
    addButtonText: 'New Mail',
    
    /**
     * @property checkMailsDelayedTask
     * @type Ext.util.DelayedTask
     */
    checkMailsDelayedTask: null,
    
    /**
     * @property defaultAccount
     * @type Tine.Felamimail.Model.Account
     */
    defaultAccount: null,
    
    /**
     * @type Ext.data.JsonStore
     */
    folderStore: null,
    
    /**
     * @type Ext.data.JsonStore
     */
    accountStore: null,
    
    /**
     * @property updateInterval user defined update interval (milliseconds)
     * @type Number
     */
    updateInterval: null,
    
    /**
     * transaction id of current update message cache request
     * @type Number
     */
    updateMessageCacheTransactionId: null,
    
    getFolderStatusTransactionInProgress: false, 
    
    /**
     * unreadcount in default account inbox
     * @type Number
     */
    unreadcountInDefaultInbox: 0,
    
    /**
     * returns title (Email)
     * 
     * @return {String}
     */
    getTitle: function() {
        return this.i18n._('Email');
    },
    
    /**
     * start delayed task to init folder store / updateFolderStore
     */
    init: function() {
        Tine.log.info('initialising app');
        this.checkMailsDelayedTask = new Ext.util.DelayedTask(this.checkMails, this);
        
        this.updateInterval = parseInt(Tine.Felamimail.registry.get('preferences').get('updateInterval')) * 60000;
        Tine.log.debug('user defined update interval is "' + this.updateInterval/1000 + '" seconds');
        
        this.defaultAccount = Tine.Felamimail.registry.get('preferences').get('defaultEmailAccount');
        Tine.log.debug('default account is "' + this.defaultAccount);
        
        if (window.isMainWindow) {
            if (Tine.Tinebase.appMgr.getActive() != this && this.updateInterval) {
                var delayTime = this.updateInterval/20;
                Tine.log.debug('start preloading mails in "' + delayTime/1000 + '" seconds');
                this.checkMailsDelayedTask.delay(delayTime);
            }
            
            this.showActiveVacation();
            var adbHook = new Tine.Felamimail.GridPanelHook({
                app: this,
                foreignAppName: 'Addressbook'
            });
            var crmHook = new Tine.Felamimail.GridPanelHook({
                app: this,
                foreignAppName: 'Crm',
                contactInRelation: true,
                relationType: 'CUSTOMER'
            });
        }
    },
    
    /**
     * show notification with active vacation information
     */
    showActiveVacation: function () {
        var accountsWithActiveVacation = this.getAccountStore().query('sieve_vacation_active', true);
        accountsWithActiveVacation.each(function(item) {
            Ext.ux.Notification.show(
                this.i18n._('Active Vacation Message'), 
                String.format(this.i18n._('Email account "{0}" has an active vacation message.'), item.get('name'))
            );
        }, this);
    },

    /**
     * check mails
     * 
     * if no folder is given, we find next folder to update ourself
     * 
     * @param {Tine.Felamimail.Model.Folder} [folder]
     * @param {Function} [callback]
     */
    checkMails: function(folder, callback) {
        this.checkMailsDelayedTask.cancel();
        
        if (! this.getFolderStore().getCount() && this.defaultAccount) {
            this.fetchSubfolders('/' + this.defaultAccount);
            return;
        }
        
        Tine.log.info('checking mails' + (folder ? ' for folder ' + folder.get('localname') : '') + ' now: ' + new Date());
        
        // if no folder is given, see if there is a folder to check in the folderstore
        if (! folder) {
            folder = this.getNextFolderToUpdate();
        }
        
        // for testing purposes
        //console.log('update disabled');
        //return;
        
        if (folder) {
            var executionTime = folder.isCurrentSelection() ? 10 : Math.min(this.updateInterval/1000, 120);
            
            if (this.updateMessageCacheTransactionId && Tine.Felamimail.folderBackend.isLoading(this.updateMessageCacheTransactionId)) {
                var currentRequestFolder = this.folderStore.query('cache_status', 'pending').first(),
                    expectedResponseIn = Math.floor((this.updateMessageCacheTransactionExpectedResponse.getTime() - new Date().getTime())/1000);
            
                if (currentRequestFolder && (currentRequestFolder !== folder || expectedResponseIn > executionTime)) {
                    Tine.log.debug('aborting current update message request (expected response in ' + expectedResponseIn + ' seconds)');
                    Tine.Felamimail.folderBackend.abort(this.updateMessageCacheTransactionId);
                    currentRequestFolder.set('cache_status', 'incomplete');
                    currentRequestFolder.commit();
                } else {
                    Tine.log.debug('a request updating message cache for folder "' + folder.get('localname') + '" is in progress -> wait for request to return');
                    return;
                }
            }
            
            Tine.log.debug('Updating message cache for folder "' + folder.get('localname') + '" of account ' + folder.get('account_id'));
            Tine.log.debug('Max execution time: ' + executionTime + ' seconds');
            
            this.updateMessageCacheTransactionExpectedResponse = new Date().add(Date.SECOND, executionTime);
            folder.set('cache_status', 'pending');
            folder.commit();
            
            this.updateMessageCacheTransactionId = Tine.Felamimail.folderBackend.updateMessageCache(folder.id, executionTime, {
                scope: this,
                callback: callback,
                failure: this.onBackgroundRequestFail,
                success: function(folder) {
                    this.getAccountStore().getById(folder.get('account_id')).setLastIMAPException(null);
                    this.getFolderStore().updateFolder(folder);
                    
                    if (folder.get('cache_status') === 'updating') {
                        Tine.log.debug('updating message cache for folder "' + folder.get('localname') + '" is in progress on the server (folder is locked)');
                        return this.checkMailsDelayedTask.delay(this.updateInterval);
                    }
                    this.checkMailsDelayedTask.delay(0);
                }
            });
        } else {
            var allFoldersFetched = this.fetchSubfolders();
            
            if (allFoldersFetched) {
                Tine.log.info('nothing more to do -> will check mails again in "' + this.updateInterval/1000 + '" seconds');
                if (this.updateInterval > 0) {
                    this.checkMailsDelayedTask.delay(this.updateInterval);
                }
            } else {
                this.checkMailsDelayedTask.delay(20000);
            }
        }
    },
    
    /**
     * fetch subfolders by parent path 
     * - if parentPath param is empty, it loops all accounts and account folders to find the next folders to fetch
     * 
     * @param {String} parentPath
     * @return {Boolean} true if all folders of all accounts have been fetched
     */
    fetchSubfolders: function(parentPath) {
        var folderStore = this.getFolderStore(),
            accountStore = this.getAccountStore(),
            doQuery = true,
            allFoldersFetched = false;
        
        if (! parentPath) {
            // find first account that has unfetched folders
            var index = accountStore.findExact('all_folders_fetched', false),
                account = accountStore.getAt(index);
            
            if (account) {
                // determine the next level of folders that is not fetched
                parentPath = '/' + account.id;
                
                var recordsOfAccount = folderStore.query('account_id', account.id);
                if (recordsOfAccount.getCount() > 0) {
                    // loop account folders and find the next folder path that hasn't been queried and has children
                    var path, found = false;
                    recordsOfAccount.each(function(record) {
                        path = parentPath + '/' + record.id;
                        if (! folderStore.isLoadedOrLoading('parent_path', path) && (! account.get('has_children_support') || record.get('has_children'))) {
                            parentPath = path;
                            found = true;
                            Tine.log.debug('fetching next level of subfolders for ' + record.get('globalname'));
                            return false;
                        }
                        return true;
                    }, this);
                    
                    if (! found) {
                        Tine.log.debug('all folders of account ' + account.get('name') + ' have been fetched ...');
                        account.set('all_folders_fetched', true);
                        return false;
                    }
                } else {
                    Tine.log.debug('fetching first level of folders for account ' + account.get('name'));
                }
                
            } else {
                Tine.log.debug('all folders of all accounts have been fetched ...');
                return true;
            }
        } else {
            Tine.log.debug('no folders in store yet, fetching first level ...');
        }
        
        if (! folderStore.queriesPending || folderStore.queriesPending.length == 0) {
            folderStore.asyncQuery('parent_path', parentPath, this.checkMails.createDelegate(this, []), [], this, folderStore);
        } else {
            this.checkMailsDelayedTask.delay(0);
        }
        
        return false;
    },
    
    /**
     * get folder store
     * 
     * @return {Tine.Felamimail.FolderStore}
     */
    getFolderStore: function() {
        if (! this.folderStore) {
            Tine.log.debug('creating folder store');
            this.folderStore = new Tine.Felamimail.FolderStore({
                listeners: {
                    scope: this,
                    update: this.onUpdateFolder
                }
            });
        }
        
        return this.folderStore;
    },
    
    /**
     * gets next folder which needs to be checked for mails
     * 
     * @return {Model.Folder/null}
     */
    getNextFolderToUpdate: function() {
        var currNode = this.getMainScreen().getTreePanel().getSelectionModel().getSelectedNode(),
            currFolder = currNode ? this.getFolderStore().getById(currNode.id) : null;
        
        // current selection has highest prio!
        if (currFolder && currFolder.needsUpdate(this.updateInterval)) {
            return currFolder;
        }
        
        // check if inboxes need updates
        var inboxes = this.folderStore.queryBy(function(folder) {
            return folder.isInbox() && folder.needsUpdate(this.updateInterval);
        }, this);
        if (inboxes.getCount() > 0) {
            return inboxes.first();
        }
        
        // check for incompletes
        var incompletes = this.folderStore.queryBy(function(folder) {
            return (['complete', 'updating', 'disconnect'].indexOf(folder.get('cache_status')) === -1 && folder.get('is_selectable'));
        }, this);
        if (incompletes.getCount() > 0) {
            Tine.log.debug('Got ' + incompletes.getCount() + ' incomplete folders.');
            var firstIncomplete = incompletes.first();
            Tine.log.debug('First ' + firstIncomplete.get('cache_status') + ' folder to check: ' + firstIncomplete.get('globalname'));
            return firstIncomplete;
        }
        
        // check for outdated
        if (! this.getFolderStatusTransactionInProgress) {
            this.getStatusOfOutdatedFolders();
        } else {
            Tine.log.debug('getFolderStatus() already running ... wait a little more.');
        }
        
        // nothing to update
        return null;
    },
    
    /**
     * collects outdated folders and calls getFolderStatus on server to fetch all folders that need to be updated
     */
    getStatusOfOutdatedFolders: function() {
        var outdated = this.folderStore.queryBy(function(folder) {
            if (! folder.get('is_selectable')) {
                return false;
            }
            
            var timestamp = folder.get('client_access_time');
            if (! Ext.isDate(timestamp)) {
                return true;
            }
            // update inboxes more often than other folders
            if (folder.isInbox() && timestamp.getElapsed() > this.updateInterval) {
                return true;
            } else if (timestamp.getElapsed() > (this.updateInterval * 5)) {
                return true;
            }
            return false;
        }, this);
        
        if (outdated.getCount() > 0) {
            Tine.log.debug('Still got ' + outdated.getCount() + ' outdated folders to update');
            
            // call Felamimail.getFolderStatus() with ids of outdated folders -> update folder store on success
            // get only max 50 folders at once
            var rangeOfFolders = (outdated.getCount() > 50) ? outdated.getRange(0, 49) : outdated.getRange(),
                ids = [],
                now = new Date();
            Ext.each(rangeOfFolders, function(folder) {
                folder.set('client_access_time', now);
                ids.push(folder.id);
            });
            
            var filter = [{field: 'id', operator: 'in', value: ids}];
            Tine.log.debug('Requesting folder status of ' + rangeOfFolders.length + ' folders ...');
            Tine.Felamimail.getFolderStatus(filter, this.onGetFolderStatusSuccess.createDelegate(this));
            this.getFolderStatusTransactionInProgress = true;
        }
    },
    
    /**
     * get folder status returned -> set folders that need an update to pending status
     * 
     * @param {Array} response
     */
    onGetFolderStatusSuccess: function(response) {
        this.getFolderStatusTransactionInProgress = false;
        Tine.log.debug('Tine.Felamimail.Application::onGetFolderStatusSuccess() -> Folder status update successful.');
        Tine.log.debug(response);
        
        if (response && response.length > 0) {
            Tine.log.debug('Tine.Felamimail.Application::onGetFolderStatusSuccess() -> Got ' + response.length + ' folders that need an update.');
            
            Ext.each(response, function(folder) {
                var folderToUpdate = this.folderStore.getById(folder.id);
                folderToUpdate.set('cache_status', 'pending');
            }, this);
            
            this.checkMailsDelayedTask.delay(1000);
        } else {
            Tine.log.debug('Tine.Felamimail.Application::onGetFolderStatusSuccess() -> No folders for update found.');
        }
    },
    
    /**
     * executed when updateMessageCache requests fail
     * 
     * NOTE: We show the credential error dlg and this only for the first error
     * 
     * @param {Object} exception
     */
    onBackgroundRequestFail: function(exception) {
        var currentRequestFolder = this.folderStore.query('cache_status', 'pending').first();
        var accountId     = currentRequestFolder.get('account_id'),
            account       = accountId ? this.getAccountStore().getById(accountId): null,
            imapStatus    = account ? account.get('imap_status') : null,
            grid          = this.getMainScreen().getCenterPanel(),
            manualRefresh = grid && grid.manualRefresh;
        
        if (manualRefresh) {
            grid.manualRefresh = false;
            grid.pagingToolbar.refresh.enable();
        }
        
        if (exception.code == 913) {
            // folder not found -> remove folder from store and tree panel
            var treePanel = this.getMainScreen().getTreePanel(),
                node = treePanel.getNodeById(currentRequestFolder.id);
            if (node) {
                node.remove();
            }
            this.getFolderStore().remove(currentRequestFolder);
        } else if (account && (manualRefresh ||
            //  do not show exclamation mark for timeouts and connection losses
            (exception.code !== 520 && exception.code !== 510))
        ) {
            account.setLastIMAPException(exception);
            
            this.getFolderStore().each(function(folder) {
                if (folder.get('account_id') === accountId) {
                    folder.set('cache_status', 'disconnect');
                    folder.commit();
                }
            }, this);
            
            if (exception.code == 912 && imapStatus !== 'failure' && Tine.Tinebase.appMgr.getActive() === this) {
                Tine.Felamimail.folderBackend.handleRequestException(exception);
            }
        }
        
        Tine.log.info((manualRefresh ? 'Manual' : 'Background') + ' update failed (' + exception.message
            + ') for folder ' + currentRequestFolder.get('globalname') 
            + ' -> will check mails again in "' + this.updateInterval/1000 + '" seconds');
        Tine.log.debug(exception);
        this.checkMailsDelayedTask.delay(this.updateInterval);
    },
    
    /**
     * executed right before this app gets activated
     */
    onBeforeActivate: function() {
        Tine.log.info('activating felamimail now');
        // abort preloading/old actions and force fresh fetch
        this.checkMailsDelayedTask.delay(0);
    },
    
    /**
     * on update folder
     * 
     * @param {Tine.Felamimail.FolderStore} store
     * @param {Tine.Felamimail.Model.Folder} record
     * @param {String} operation
     */
    onUpdateFolder: function(store, record, operation) {
        if (operation === Ext.data.Record.EDIT) {
            if (record.isModified('cache_status')) {
                Tine.log.info('Tine.Felamimail.Application::onUpdateFolder(): Folder "' + record.get('localname') + '" updated with cache_status: ' + record.get('cache_status'));
                
                // as soon as we get a folder with status != complete we need to trigger checkmail soon!
                if (['complete', 'pending'].indexOf(record.get('cache_status')) === -1) {
                    this.checkMailsDelayedTask.delay(1000);
                }
                
                if (record.isInbox() && record.isModified('cache_unreadcount')) {
                    this.showNewMessageNotification(record);
                }
            }

            if (record.isInbox()) {
                if (this.isDefaultAccountId(record.get('account_id'))) {
                    if (record.isModified('cache_unreadcount') || record.get('cache_unreadcount') != this.unreadcountInDefaultInbox) {
                        this.setTitleWithUnreadcount(record.get('cache_unreadcount'));
                    }
                }
                
                if (record.isModified('quota_usage') || record.isModified('quota_limit')) {
                    this.onUpdateFolderQuota(record);
                }
            }
        }
    },
    
    /**
     * checks default account id
     * 
     * @param {String} accountId
     * @return {Boolean}
     */
    isDefaultAccountId: function(accountId) {
        return accountId == Tine.Felamimail.registry.get('preferences').get('defaultEmailAccount');
    },
    
    /**
     * show notification for new messages
     * 
     * @param {Tine.Felamimail.Model.Folder} record
     */
    showNewMessageNotification: function(record) {
        var recents = (record.get('cache_unreadcount') - record.modified.cache_unreadcount),
            account = this.getAccountStore().getById(record.get('account_id'));
            
        if (recents > 0 ) {
            Tine.log.info('Show notification: ' + recents + ' new mails.');
            var title = this.i18n._('New mails'),
                message = String.format(this.i18n._('You got {0} new mail(s) in folder {1} ({2}).'), recents, record.get('localname'), account.get('name'));
            
            if (record.isCurrentSelection()) {
                // need to defer the notification because the new messages are not shown yet 
                // -> improve this with a callback fn or something like that / unread count should be updated when the messages become visible, too
                Ext.ux.Notification.show.defer(3500, this, [title, message]);
            } else {
                Ext.ux.Notification.show(title, message);
            }
        }
    },
    
    /**
     * write number of unread messages in all accounts into title
     * 
     * @param {Number} unreadcount
     */
    setTitleWithUnreadcount: function(unreadcount) {
        if (! window.isMainWindow) {
            return;
        }

        this.unreadcountInDefaultInbox = unreadcount;
        if (this.unreadcountInDefaultInbox < 0) {
            this.unreadcountInDefaultInbox = 0;
        }
        
        Tine.log.info('Updating title with new unreadcount: ' + this.unreadcountInDefaultInbox);
        var currentTitle = document.title,
            unreadString = (this.unreadcountInDefaultInbox != 0) ? '(' + this.unreadcountInDefaultInbox + ') ' : '';
            
        if (currentTitle.match(/^\([0-9]+\) /)) {
            document.title = document.title.replace(/^\([0-9]+\) /, unreadString);
        } else {
            document.title = unreadString + currentTitle;
        }
    },
    
    /**
     * folder quota is updated
     * 
     * @param {Tine.Felamimail.Model.Folder} record
     */
    onUpdateFolderQuota: function(record) {
        if (record.get('quota_usage')) {
            Tine.log.info('Folder "' + record.get('localname') + '" updated with quota values: ' 
                + record.get('quota_usage') + ' / ' + record.get('quota_limit'));

            this.getMainScreen().getCenterPanel().updateQuotaBar(record);
        }
    },
    
    /**
     * get active account
     * @return {Tine.Felamimail.Model.Account}
     */
    getActiveAccount: function() {
        var account = null;
            
        var treePanel = this.getMainScreen().getTreePanel();
        if (treePanel && treePanel.rendered) {
            account = treePanel.getActiveAccount();
        }
        
        if (account === null) {
            account = this.getAccountStore().getById(Tine.Felamimail.registry.get('preferences').get('defaultEmailAccount'));
        }
        
        if (account === null) {
            // try to get first account in store
            account = this.getAccountStore().getAt(0);
        }
        
        return account;
    },
    
    /**
     * get account store
     * 
     * @return {Ext.data.JsonStore}
     */
    getAccountStore: function() {
        if (! this.accountStore) {
            Tine.log.debug('creating account store');
            
            // create store (get from initial data)
            this.accountStore = new Ext.data.JsonStore({
                fields: Tine.Felamimail.Model.Account,
                data: Tine.Felamimail.registry.get('accounts'),
                autoLoad: true,
                id: 'id',
                root: 'results',
                totalProperty: 'totalcount',
                proxy: Tine.Felamimail.accountBackend,
                reader: Tine.Felamimail.accountBackend.getReader(),
                listeners: {
                    scope: this,
                    'add': function (store, records) {
                        Tine.log.info('Account added: ' + records[0].get(Tine.Felamimail.Model.Account.getMeta('titleProperty')));
                        this.getMainScreen().getCenterPanel().action_write.setDisabled(! this.getActiveAccount());
                    },
                    'remove': function (store, record) {
                        Tine.log.info('Account removed: ' + record.get(Tine.Felamimail.Model.Account.getMeta('titleProperty')));
                        this.getMainScreen().getCenterPanel().action_write.setDisabled(! this.getActiveAccount());
                    }
                }
            });
        } 
    
        return this.accountStore;
    },
    
    /**
     * show felamimail credentials dialog
     * 
     * @param {Tine.Felamimail.Model.Account} account
     * @param {String} username [optional]
     */
    showCredentialsDialog: function(account, username) {
        Tine.Felamimail.credentialsDialog = Tine.widgets.dialog.CredentialsDialog.openWindow({
            windowTitle: String.format(this.i18n._('IMAP Credentials for {0}'), account.get('name')),
            appName: 'Felamimail',
            credentialsId: account.id,
            i18nRecordName: this.i18n._('Credentials'),
            recordClass: Tine.Tinebase.Model.Credentials,
            record: new Tine.Tinebase.Model.Credentials({
                id: account.id,
                username: username ? username : ''
            }),
            listeners: {
                scope: this,
                'update': function(data) {
                    var folderStore = this.getFolderStore();
                    if (folderStore.queriesPending.length > 0) {
                        // reload all folders of account and try to select inbox
                        var accountId = folderStore.queriesPending[0].substring(16, 56),
                            account = this.getAccountStore().getById(accountId),
                            accountNode = this.getMainScreen().getTreePanel().getNodeById(accountId);
                            
                        folderStore.resetQueryAndRemoveRecords('parent_path', '/' + accountId);
                        account.set('all_folders_fetched', true);
                        
                        accountNode.loading = false;
                        accountNode.reload(function(callback) {
                            Ext.each(accountNode.childNodes, function(node) {
                                if (Ext.util.Format.lowercase(node.attributes.localname) == 'inbox') {
                                    node.select();
                                    return false;
                                }
                            }, this);
                        });
                    } else {
                        this.checkMailsDelayedTask.delay(0);
                    }
                }
            }
        });
    }
});

/**
 * @namespace Tine.Felamimail
 * @class Tine.Felamimail.MainScreen
 * @extends Tine.widgets.MainScreen
 * 
 * MainScreen Definition
 */ 
Tine.Felamimail.MainScreen = Ext.extend(Tine.widgets.MainScreen, {
    /**
     * adapter fn to get folder tree panel
     * 
     * @return {Ext.tree.TreePanel}
     */
    getTreePanel: function() {
        return this.getWestPanel().getContainerTreePanel();
    }
});

/**
 * get flags store
 *
 * @param {Boolean} reload
 * @return {Ext.data.JsonStore}
 */
Tine.Felamimail.loadFlagsStore = function(reload) {
    
    var store = Ext.StoreMgr.get('FelamimailFlagsStore');
    
    if (!store) {
        // create store (get from initial registry data)
        store = new Ext.data.JsonStore({
            fields: Tine.Felamimail.Model.Flag,
            data: Tine.Felamimail.registry.get('supportedFlags'),
            autoLoad: true,
            id: 'id',
            root: 'results',
            totalProperty: 'totalcount'
        });
        
        Ext.StoreMgr.add('FelamimailFlagsStore', store);
    } 

    return store;
};

/**
 * add signature (get it from default account settings)
 * 
 * @param {String} id
 * @return {String}
 */
Tine.Felamimail.getSignature = function(id) {
    
    var result = '',
        app = Tine.Tinebase.appMgr.get('Felamimail'),
        activeAccount = app.getMainScreen().getTreePanel().getActiveAccount();
        
    id = id || (activeAccount ? activeAccount.id : 'default');
    
    if (id === 'default') {
        id = Tine.Felamimail.registry.get('preferences').get('defaultEmailAccount');
    }
    
    var defaultAccount = app.getAccountStore().getById(id);
    var signature = (defaultAccount) ? defaultAccount.get('signature') : '';
    if (signature && signature != '') {
        // NOTE: signature is always in html, nl2br here would cause duplicate linebreaks!
        result = '<br><br><span id="felamimail-body-signature">-- <br>' + signature + '</span>';
    }
    
    return result;
};

/**
 * get email string (n_fileas <email@host.tld>) from contact
 * 
 * @param {Tine.Addressbook.Model.Contact} contact
 * @return {String}
 */
Tine.Felamimail.getEmailStringFromContact = function(contact) {
    Tine.log.debug('Tine.Felamimail.getEmailStringFromContact() - getting contact email');
    Tine.log.debug(contact);
    
    var result = contact.get('n_fileas') + ' <';
    result += contact.getPreferedEmail();
    result += '>';
    
    return result;
};

/**
 * generic exception handler for felamimail (used by folder and message backends and updateMessageCache)
 * 
 * TODO move all 902 exception handling here!
 * TODO invent requery on 902 with cred. dialog
 * 
 * @param {Tine.Exception|Object} exception
 */
Tine.Felamimail.handleRequestException = function(exception) {
    if (! exception.code && exception.responseText) {
        // we need to decode the exception first
        var response = Ext.util.JSON.decode(exception.responseText);
        exception = response.data;
    }

    Tine.log.warn('Request exception :');
    Tine.log.warn(exception);
    
    var app = Tine.Tinebase.appMgr.get('Felamimail');
    
    switch(exception.code) {
        case 910: // Felamimail_Exception_IMAP
        case 911: // Felamimail_Exception_IMAPServiceUnavailable
            Ext.Msg.show({
               title:   app.i18n._('IMAP Error'),
               msg:     exception.message ? exception.message : app.i18n._('No connection to IMAP server.'),
               icon:    Ext.MessageBox.ERROR,
               buttons: Ext.Msg.OK
            });
            break;
            
        case 912: // Felamimail_Exception_IMAPInvalidCredentials
            var accountId   = exception.account && exception.account.id ? exception.account.id : '',
                account     = accountId ? app.getAccountStore().getById(accountId): null,
                imapStatus  = account ? account.get('imap_status') : null;
                
            if (account) {
                account.set('all_folders_fetched', true);
                if (account.get('type') == 'system') {
                    // just show message box for system accounts
                    Ext.Msg.show({
                       title:   app.i18n._('IMAP Credentials Error'),
                       msg:     app.i18n._('Your email credentials are wrong. Please contact your administrator'),
                       icon:    Ext.MessageBox.ERROR,
                       buttons: Ext.Msg.OK
                    });
                } else {
                    app.showCredentialsDialog(account, exception.username);
                }
            } else {
                exception.code = 910;
                return this.handleRequestException(exception);
            }
            break;
            
        case 913: // Felamimail_Exception_IMAPFolderNotFound
            Ext.Msg.show({
               title:   app.i18n._('IMAP Error'),
               msg:     app.i18n._('One of your folders was deleted or renamed by another client. Please update the folder list of this account.'),
               icon:    Ext.MessageBox.ERROR,
               buttons: Ext.Msg.OK
            });
            // TODO reload account root node
            break;
            
        case 914: // Felamimail_Exception_IMAPMessageNotFound
            Tine.log.notice('Message was deleted by another client.');
            
            // remove message from store and select next message
            var requestParams = Ext.util.JSON.decode(exception.request).params,
                centerPanel = app.getMainScreen().getCenterPanel(),
                msg = centerPanel.getStore().getById(requestParams.id);
                
            if (msg) {
                var sm = centerPanel.getGrid().getSelectionModel(),
                    selectedMsgs = sm.getSelectionsCollection(),
                    nextMessage = centerPanel.getNextMessage(selectedMsgs);
                    
                centerPanel.getStore().remove(msg);
                if (nextMessage) {
                    sm.selectRecords([nextMessage]);
                }
            }
            break;
            
        case 920: // Felamimail_Exception_SMTP
            Ext.Msg.show({
               title:   app.i18n._('SMTP Error'),
               msg:     exception.message ? exception.message : app.i18n._('No connection to SMTP server.'),
               icon:    Ext.MessageBox.ERROR,
               buttons: Ext.Msg.OK
            });
            break;
            
        case 930: // Felamimail_Exception_Sieve
            Ext.Msg.show({
               title:   app.i18n._('Sieve Error'),
               msg:     exception.message ? exception.message : app.i18n._('No connection to Sieve server.'),
               icon:    Ext.MessageBox.ERROR,
               buttons: Ext.Msg.OK
            });
            break;

        case 931: // Felamimail_Exception_SievePutScriptFail
            Ext.Msg.show({
               title:   app.i18n._('Save Sieve Script Error'),
               msg:     app.i18n._('Could not save script on Sieve server.') + (exception.message ? ' (' + exception.message + ')' : ''),
               icon:    Ext.MessageBox.ERROR,
               buttons: Ext.Msg.OK
            });
            break;

        default:
            Tine.Tinebase.ExceptionHandler.handleRequestException(exception);
            break;
    }
};
/*
 * Tine 2.0
 * 
 * @package     Felamimail
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2010-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */
 
Ext.namespace('Tine.Felamimail');

/**
 * @namespace   Tine.Felamimail
 * @class       Tine.Felamimail.ComposeEditor
 * @extends     Ext.form.HtmlEditor
 * 
 * <p>Compose HTML Editor</p>
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * 
 * @param       {Object} config
 * @constructor
 * Create a new Tine.Felamimail.ComposeEditor
 */
Tine.Felamimail.ComposeEditor = Ext.extend(Ext.form.HtmlEditor, {
    
    cls: 'felamimail-message-body-html',
    name: 'body',
    allowBlank: true,

    getDocMarkup: function(){
        var markup = '<html>'
            + '<head>'
            + '<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">'
            + '<title></title>'
            + '<style type="text/css">'
                // standard css reset
                + "html,body,div,dl,dt,dd,ul,ol,li,h1,h2,h3,h4,h5,h6,pre,form,fieldset,input,p,blockquote,th,td{margin:0;padding:0;}img,body,html{border:0;}address,caption,cite,code,dfn,em,strong,th,var{font-style:normal;font-weight:normal;}ol,ul {list-style:none;}caption,th {text-align:left;}h1,h2,h3,h4,h5,h6{font-size:100%;}q:before,q:after{content:'';}"
                // small forms
                + "html,body,div,dl,dt,dd,ul,ol,li,h1,h2,h3,h4,h5,h6,pre,form,fieldset,input,p,blockquote,th,td{font-size: small;}"
                // lists
                + "ul {list-style:circle outside; margin-left: 20px;}"
                + "ol {list-style:decimal outside; margin-left: 20px;}"
                // fmail special
                + '.felamimail-body-blockquote {'
                    + 'margin: 5px 10px 0 3px;'
                    + 'padding-left: 10px;'
                    + 'border-left: 2px solid #000088;'
                + '} '
            + '</style>'
            + '</head>'
            + '<body style="padding: 5px 0px 0px 5px; margin: 0px">'
            + '</body></html>';

        return markup;
    },
    
    /**
     * @private
     */
    initComponent: function() {
        
        this.plugins = [
            new Ext.ux.form.HtmlEditor.IndentOutdent(),  
            new Ext.ux.form.HtmlEditor.RemoveFormat(),
            new Ext.ux.form.HtmlEditor.EndBlockquote(),
            new Ext.ux.form.HtmlEditor.SpecialKeys()
        ];
        
        Tine.Felamimail.ComposeEditor.superclass.initComponent.call(this);
    },
         
    // *Fix* Overridding the onRender method, in order to
    // unset the height and width property, so that the
    // layout manager won't consider this field to be of
    // fixed dimension, thus ignoring the flex option
    onRender: function () {
        Tine.Felamimail.ComposeEditor.superclass.onRender.apply(this, arguments);
        delete this.height;
        delete this.width;
    }
});

Ext.namespace('Ext.ux.form.HtmlEditor');

/**
 * @class Ext.ux.form.HtmlEditor.EndBlockquote
 * @extends Ext.util.Observable
 * 
 * plugin for htmleditor that ends blockquotes on ENTER
 * tested with chrome, sarari, FF4+
 * fallsback for old (non IE) browser which works for easy structures
 * does not work with IE yet
 * 
 * TODO move this to ux dir
 */
Ext.ux.form.HtmlEditor.EndBlockquote = Ext.extend(Ext.util.Observable , {

    // private
    init: function(cmp){
            this.cmp = cmp;
            this.cmp.on('initialize', this.onInit, this);
    },
    
    // private
    onInit: function() {
        if (Ext.isIE) {
            Ext.EventManager.on(this.cmp.getDoc(), {
                'keydown': this.endBlockquoteIE,
                scope: this
            });
        } else if (Ext.isFunction(this.cmp.win.getSelection().modify)) {
            Ext.EventManager.on(this.cmp.getDoc(), {
                'keyup': this.endBlockquoteHTML5,
                scope: this
            });
        } else {
            Ext.EventManager.on(this.cmp.getDoc(), {
                'keydown': this.endBlockquoteHTML4,
                scope: this
            });
        }
        
    },

    /**
     * on keyup 
     * 
     * @param {Event} e
     */
    endBlockquoteIE: function(e) {
        if (e.getKey() == e.ENTER) {
            
            e.stopEvent();
            e.preventDefault();
            
            var s = this.cmp.win.document.selection,
                r = s.createRange(),
                doc = this.cmp.getDoc(),
                anchor = r.parentElement(),
                level = this.getBlockquoteLevel(anchor),
                scrollTop = doc.body.scrollTop;
                
            if (level > 0) {
                r.moveStart('word', -1);
                r.moveEnd('textedit');
                var fragment = r.htmlText;
                r.execCommand('Delete');
                
                var newText = doc.createElement('p'),
                    newTextMark = '###newTextHere###' + Ext.id(),
                    fragmentMark = '###fragmentHere###' + Ext.id();
                newText.innerHTML = newTextMark + fragmentMark;
                doc.body.appendChild(newText);
                
                r.expand('textedit');
                r.findText(fragmentMark);
                r.select();
                r.pasteHTML(fragment);

                r.expand('textedit');
                r.findText(newTextMark);
                r.select();
                r.execCommand('Delete');
                
                // reset scroller
                doc.body.scrollTop = scrollTop;
                
                this.cmp.syncValue();
                this.cmp.deferFocus();
            }
            
        }
        
        return;
    },
    
    /**
     * on keyup 
     * 
     * @param {Event} e
     */
    endBlockquoteHTML5: function(e) {
        // Chrome, Safari, FF4+
        if (e.getKey() == e.ENTER) {
            var s = this.cmp.win.getSelection(),
                r = s.getRangeAt(0),
                doc = this.cmp.getDoc(),
                level = this.getBlockquoteLevel(s.anchorNode),
                scrollTop = doc.body.scrollTop;
                
            if (level > 0) {
                // cut from cursor to end of the document
                if (s.anchorNode.nodeName == '#text') {
                    r.setStartBefore(s.anchorNode.parentNode);
                }
                s.modify("move", "backward", "character");
                r.setEndAfter(doc.body.lastChild);
                var fragmet = r.extractContents();
                
                // insert paragraph for new text and move cursor in
                // NOTE: we need at least one character in the newText to move cursor in
                var newText = doc.createElement('p');
                newText.innerHTML = '&nbsp;';
                doc.body.appendChild(newText);
                s.modify("move", "forward", "character");
                
                // paste rest of document 
                doc.body.appendChild(fragmet);
                
                // reset scroller
                doc.body.scrollTop = scrollTop;
            }
        }
    },

    /**
     * on keydown 
     * 
     * @param {Event} e
     */
    endBlockquoteHTML4: function(e) {
        if (e.getKey() == e.ENTER) {
            var s = this.cmp.win.getSelection(),
                r = s.getRangeAt(0),
                doc = this.cmp.getDoc(),
                level = this.getBlockquoteLevel(s.anchorNode);
            
            if (level > 0) {
                e.stopEvent();
                e.preventDefault();
                
                this.cmp.win.focus();
                if (level == 1) {
                    this.cmp.insertAtCursor('<br /><blockquote class="felamimail-body-blockquote"><br />');
                    this.cmp.execCmd('outdent');
                    this.cmp.execCmd('outdent');
                } else if (level > 1) {
                    for (var i=0; i < level; i++) {
                        this.cmp.insertAtCursor('<br /><blockquote class="felamimail-body-blockquote">');
                        this.cmp.execCmd('outdent');
                        this.cmp.execCmd('outdent');
                    }
                    var br = doc.createElement('br');
                    r.insertNode(br);
                }
                this.cmp.deferFocus();
            }
        }
    },
    
    /**
     * get blockquote level helper
     * 
     * @param {DOMNode} node
     * @return {Integer}
     */
    getBlockquoteLevel: function(node) {
        var result = 0;
        
        while (node.nodeName == '#text' || node.tagName.toLowerCase() != 'body') {
            if (node.tagName && node.tagName.toLowerCase() == 'blockquote') {
                result++;
            }
            node = node.parentNode ? node.parentNode : node.parentElement;
        }
        
        return result;
    }
});

/**
 * @class Ext.ux.form.HtmlEditor.SpecialKeys
 * @extends Ext.util.Observable
 * 
 * plugin for htmleditor that fires events for special keys (like CTRL-ENTER and SHIFT-TAB)
 * 
 * TODO move this to ux dir
 */
Ext.ux.form.HtmlEditor.SpecialKeys = Ext.extend(Ext.util.Observable , {
    // private
    init: function(cmp){
        this.cmp = cmp;
        this.cmp.on('initialize', this.onInit, this);
    },
    // private
    onInit: function(){
        Ext.EventManager.on(this.cmp.getDoc(), {
            'keydown': this.onKeydown,
            scope: this
        });
    },

    /**
     * on keydown 
     * 
     * @param {Event} e
     * 
     * TODO try to prevent TAB key event from inserting a TAB in the editor 
     */
    onKeydown: function(e) {
        if (e.getKey() == e.TAB && e.shiftKey || e.getKey() == e.ENTER && e.ctrlKey) {
            this.cmp.fireEvent('keydown', e);
        }
    }
});
/*
 * Tine 2.0
 * 
 * @package     Addressbook
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 * @copyright   Copyright (c) 2007-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */
 
Ext.ns('Tine.Addressbook');

/**
 * Contact grid panel
 * 
 * @namespace   Tine.Addressbook
 * @class       Tine.Addressbook.ContactGridPanel
 * @extends     Tine.widgets.grid.GridPanel
 * 
 * <p>Contact Grid Panel</p>
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Cornelius Weiss <c.weiss@metaways.de>
 * @copyright   Copyright (c) 2007-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 * 
 * @param       {Object} config
 * @constructor
 * Create a new Tine.Addressbook.ContactGridPanel
 */
Tine.Addressbook.ContactGridPanel = Ext.extend(Tine.widgets.grid.GridPanel, {
    /**
     * record class
     * @cfg {Tine.Addressbook.Model.Contact} recordClass
     */
    recordClass: Tine.Addressbook.Model.Contact,
    
    /**
     * grid specific
     * @private
     */ 
    defaultSortInfo: {field: 'n_fileas', direction: 'ASC'},
    gridConfig: {
        autoExpandColumn: 'n_fileas',
        enableDragDrop: true,
        ddGroup: 'containerDDGroup'
    },
    copyEditAction: true,
    felamimail: false,
    multipleEdit: true,
    duplicateResolvable: true,
    
    /**
     * @cfg {Bool} hasDetailsPanel 
     */
    hasDetailsPanel: true,
    
    /**
     * inits this cmp
     * @private
     */
    initComponent: function() {
        this.recordProxy = Tine.Addressbook.contactBackend;
        
        // check if felamimail is installed and user has run right and wants to use felamimail in adb
        if (Tine.Felamimail && Tine.Tinebase.common.hasRight('run', 'Felamimail') && Tine.Felamimail.registry.get('preferences').get('useInAdb')) {
            this.felamimail = (Tine.Felamimail.registry.get('preferences').get('useInAdb') == 1);
        }
        this.gridConfig.cm = this.getColumnModel();
        this.filterToolbar = this.filterToolbar || this.getFilterToolbar();

        if (this.hasDetailsPanel) {
            this.detailsPanel = this.getDetailsPanel();
        }
        
        this.plugins = this.plugins || [];
        this.plugins.push(this.filterToolbar);
        
        Tine.Addressbook.ContactGridPanel.superclass.initComponent.call(this);
    },
    
    /**
     * returns column model
     * 
     * @return Ext.grid.ColumnModel
     * @private
     */
    getColumnModel: function() {
        return new Ext.grid.ColumnModel({
            defaults: {
                sortable: true,
                hidden: true,
                resizable: true
            },
            columns: this.getColumns()
        });
    },
    
    /**
     * returns array with columns
     * 
     * @return {Array}
     */
    getColumns: function() {
        return [
            { id: 'tid', header: this.app.i18n._('Type'), dataIndex: 'tid', width: 30, renderer: this.contactTidRenderer.createDelegate(this), hidden: false },
            { id: 'tags', header: this.app.i18n._('Tags'), dataIndex: 'tags', width: 50, renderer: Tine.Tinebase.common.tagsRenderer, sortable: false, hidden: false  },
            { id: 'salutation', header: this.app.i18n._('Salutation'), dataIndex: 'salutation', renderer: Tine.Tinebase.widgets.keyfield.Renderer.get('Addressbook', 'contactSalutation') },
            { id: 'n_family', header: this.app.i18n._('Last Name'), dataIndex: 'n_family' },
            { id: 'n_given', header: this.app.i18n._('First Name'), dataIndex: 'n_given', width: 80 },
            { id: 'n_fn', header: this.app.i18n._('Full Name'), dataIndex: 'n_fn' },
            { id: 'n_fileas', header: this.app.i18n._('Display Name'), dataIndex: 'n_fileas', hidden: false},
            { id: 'org_name', header: this.app.i18n._('Company'), dataIndex: 'org_name', width: 120, hidden: false },
            { id: 'org_unit', header: this.app.i18n._('Unit'), dataIndex: 'org_unit'  },
            { id: 'title', header: this.app.i18n._('Job Title'), dataIndex: 'title' },
            { id: 'role', header: this.app.i18n._('Job Role'), dataIndex: 'role' },
            { id: 'room', header: this.app.i18n._('Room'), dataIndex: 'room' },
            { id: 'adr_one_street', header: this.app.i18n._('Street'), dataIndex: 'adr_one_street' },
            { id: 'adr_one_locality', header: this.app.i18n._('City'), dataIndex: 'adr_one_locality', width: 150, hidden: false },
            { id: 'adr_one_region', header: this.app.i18n._('Region'), dataIndex: 'adr_one_region' },
            { id: 'adr_one_postalcode', header: this.app.i18n._('Postalcode'), dataIndex: 'adr_one_postalcode' },
            { id: 'adr_one_countryname', header: this.app.i18n._('Country'), dataIndex: 'adr_one_countryname' },
            { id: 'adr_two_street', header: this.app.i18n._('Street (private)'), dataIndex: 'adr_two_street' },
            { id: 'adr_two_locality', header: this.app.i18n._('City (private)'), dataIndex: 'adr_two_locality' },
            { id: 'adr_two_region', header: this.app.i18n._('Region (private)'), dataIndex: 'adr_two_region' },
            { id: 'adr_two_postalcode', header: this.app.i18n._('Postalcode (private)'), dataIndex: 'adr_two_postalcode' },
            { id: 'adr_two_countryname', header: this.app.i18n._('Country (private)'), dataIndex: 'adr_two_countryname' },
            { id: 'email', header: this.app.i18n._('Email'), dataIndex: 'email', width: 150, hidden: false },
            { id: 'tel_work', header: this.app.i18n._('Phone'), dataIndex: 'tel_work', hidden: false },
            { id: 'tel_cell', header: this.app.i18n._('Mobile'), dataIndex: 'tel_cell', hidden: false },
            { id: 'tel_fax', header: this.app.i18n._('Fax'), dataIndex: 'tel_fax' },
            { id: 'tel_car', header: this.app.i18n._('Car phone'), dataIndex: 'tel_car' },
            { id: 'tel_pager', header: this.app.i18n._('Pager'), dataIndex: 'tel_pager' },
            { id: 'tel_home', header: this.app.i18n._('Phone (private)'), dataIndex: 'tel_home' },
            { id: 'tel_fax_home', header: this.app.i18n._('Fax (private)'), dataIndex: 'tel_fax_home' },
            { id: 'tel_cell_private', header: this.app.i18n._('Mobile (private)'), dataIndex: 'tel_cell_private' },
            { id: 'email_home', header: this.app.i18n._('Email (private)'), dataIndex: 'email_home' },
            { id: 'url', header: this.app.i18n._('Web'), dataIndex: 'url' },
            { id: 'url_home', header: this.app.i18n._('URL (private)'), dataIndex: 'url_home' },
            { id: 'note', header: this.app.i18n._('Note'), dataIndex: 'note' },
            { id: 'tz', header: this.app.i18n._('Timezone'), dataIndex: 'tz' },
            { id: 'geo', header: this.app.i18n._('Geo'), dataIndex: 'geo' },
            { id: 'bday', header: this.app.i18n._('Birthday'), dataIndex: 'bday', renderer: Tine.Tinebase.common.dateRenderer }
        ].concat(this.getModlogColumns().concat(this.getCustomfieldColumns()));
    },
    
    /**
     * @private
     */
    initActions: function() {
        this.actions_exportContact = new Ext.Action({
            requiredGrant: 'exportGrant',
            text: String.format(this.app.i18n.ngettext('Export {0}', 'Export {0}', 50), this.i18nRecordsName),
            singularText: String.format(this.app.i18n.ngettext('Export {0}', 'Export {0}', 1), this.i18nRecordName),
            pluralText:  String.format(this.app.i18n.ngettext('Export {0}', 'Export {0}', 1), this.i18nRecordsName),
            translationObject: this.app.i18n,
            iconCls: 'action_export',
            scope: this,
            disabled: true,
            allowMultiple: true,
            menu: {
                items: [
                    new Tine.widgets.grid.ExportButton({
                        text: this.app.i18n._('Export as PDF'),
                        iconCls: 'action_exportAsPdf',
                        format: 'pdf',
                        exportFunction: 'Addressbook.exportContacts',
                        gridPanel: this
                    }),
                    new Tine.widgets.grid.ExportButton({
                        text: this.app.i18n._('Export as CSV'),
                        iconCls: 'tinebase-action-export-csv',
                        format: 'csv',
                        exportFunction: 'Addressbook.exportContacts',
                        gridPanel: this
                    }),
                    new Tine.widgets.grid.ExportButton({
                        text: this.app.i18n._('Export as ODS'),
                        format: 'ods',
                        iconCls: 'tinebase-action-export-ods',
                        exportFunction: 'Addressbook.exportContacts',
                        gridPanel: this
                    }),
                    new Tine.widgets.grid.ExportButton({
                        text: this.app.i18n._('Export as XLS'),
                        format: 'xls',
                        iconCls: 'tinebase-action-export-xls',
                        exportFunction: 'Addressbook.exportContacts',
                        gridPanel: this
                    }),
                    new Tine.widgets.grid.ExportButton({
                        text: this.app.i18n._('Export as ...'),
                        iconCls: 'tinebase-action-export-xls',
                        exportFunction: 'Addressbook.exportContacts',
                        showExportDialog: true,
                        gridPanel: this
                    })
                ]
            }
        });

        this.actions_import = new Ext.Action({
            //requiredGrant: 'addGrant',
            text: this.app.i18n._('Import contacts'),
            disabled: false,
            handler: this.onImport,
            iconCls: 'action_import',
            scope: this,
            allowMultiple: true
        });
        
        // register actions in updater
        this.actionUpdater.addActions([
            this.actions_exportContact,
            this.actions_import
        ]);
        
        Tine.Addressbook.ContactGridPanel.superclass.initActions.call(this);
    },
    
    /**
     * add custom items to action toolbar
     * 
     * @return {Object}
     */
    getActionToolbarItems: function() {
        return [
            {
                xtype: 'buttongroup',
                columns: 1,
                frame: false,
                items: [
                    this.actions_exportContact,
                    this.actions_import
                ]
            }
        ];
    },
    
    /**
     * add custom items to context menu
     * 
     * @return {Array}
     */
    getContextMenuItems: function() {
        var items = [
            '-',
            this.actions_exportContact,
            '-'
        ];
        
        return items;
    },
    
    /**
     * import contacts
     * 
     * @param {Button} btn 
     * 
     * TODO generalize this & the import button
     */
    onImport: function(btn) {
        var popupWindow = Tine.widgets.dialog.ImportDialog.openWindow({
            appName: 'Addressbook',
            modelName: 'Contact',
            defaultImportContainer: this.app.getMainScreen().getWestPanel().getContainerTreePanel().getDefaultContainer('defaultAddressbook'),
            
            // update grid after import
            listeners: {
                scope: this,
                'finish': function() {
                    this.loadGridData({
                        preserveCursor:     false, 
                        preserveSelection:  false, 
                        preserveScroller:   false,
                        removeStrategy:     'default'
                    });
                }
            }
        });
    },
        
    /**
     * tid renderer
     * 
     * @private
     * @return {String} HTML
     */
    contactTidRenderer: function(data, cell, record) {
        
        switch(record.get('type')) {
            case 'user':
                return '<img src="images/oxygen/16x16/actions/user-female.png" width="12" height="12" alt="contact" ext:qtip="' + Ext.util.Format.htmlEncode(this.app.i18n._("Internal Contact")) + '"/>';
            default:
                return "<img src='images/oxygen/16x16/actions/user.png' width='12' height='12' alt='contact'/>";
        }
    },
    
    /**
     * returns details panel
     * 
     * @private
     * @return {Tine.Addressbook.ContactGridDetailsPanel}
     */
    getDetailsPanel: function() {
        return new Tine.Addressbook.ContactGridDetailsPanel({
            gridpanel: this,
            il8n: this.app.i18n,
            felamimail: this.felamimail
        });
    }
});
/*
 * Tine 2.0
 * 
 * @package     Felamimail
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2010-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */
 
Ext.ns('Tine.Felamimail');

/**
 * Contact grid panel
 * 
 * @namespace   Tine.Felamimail
 * @class       Tine.Felamimail.ContactGridPanel
 * @extends     Tine.Addressbook.ContactGridPanel
 * 
 * <p>Contact Grid Panel</p>
 * <p>
 * </p>
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2010-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 * 
 * @param       {Object} config
 * @constructor
 * Create a new Tine.Felamimail.ContactGridPanel
 */
Tine.Felamimail.ContactGridPanel = Ext.extend(Tine.Addressbook.ContactGridPanel, {

    hasDetailsPanel: false,
    hasFavoritesPanel: false,
    hasQuickSearchFilterToolbarPlugin: false,
    stateId: 'FelamimailContactGrid',
    
    gridConfig: {
        autoExpandColumn: 'n_fileas',
        enableDragDrop: false
    },
    
    /**
     * the message record with recipients
     * @type Tine.Felamimail.Model.Message
     */
    messageRecord: null,
    
    /**
     * inits this cmp
     * @private
     */
    initComponent: function() {
        this.addEvents(
            /**
             * @event addcontacts
             * Fired when contacts are added
             */
            'addcontacts'
        );
        
        this.app = Tine.Tinebase.appMgr.get('Addressbook');
        this.initFilterToolbar();
        
        Tine.Felamimail.ContactGridPanel.superclass.initComponent.call(this);
        
        this.grid.on('rowdblclick', this.onRowDblClick, this);
        this.grid.on('cellclick', this.onCellClick, this);
        this.store.on('load', this.onContactStoreLoad, this);
    },
    
    /**
     * init filter toolbar
     */
    initFilterToolbar: function() {
        this.defaultFilters = [
            {field: 'email_query', operator: 'contains', value: '@'}
        ];
        this.filterToolbar = this.getFilterToolbar({
            filterFieldWidth: 100,
            filterValueWidth: 100
        });
    },
    
    /**
     * returns array with columns
     * 
     * @return {Array}
     */
    getColumns: function() {
        var columns = Tine.Felamimail.ContactGridPanel.superclass.getColumns.call(this);
        
        // hide all columns except name/company/email/email_home (?)
        Ext.each(columns, function(column) {
            if (['n_fileas', 'org_name', 'email'].indexOf(column.dataIndex) === -1) {
                column.hidden = true;
            }
        });
        
        this.radioTpl = new Ext.XTemplate('<input',
            ' name="' + this.id + '_{id}"',
            ' value="{type}"',
            ' type="radio"',
            ' autocomplete="off"',
            ' class="x-form-radio x-form-field"',
            ' {checked}',
        '>');
        
        Ext.each(['To', 'Cc', 'Bcc', 'None'], function(type) { // _('None')
            columns.push({
                header: this.app.i18n._(type),
                dataIndex: Ext.util.Format.lowercase(type),
                width: 50,
                hidden: false,
                renderer: this.typeRadioRenderer.createDelegate(this, [type], 0)
            });
            
        }, this);
            
        return columns;
    },
    
    /**
     * render type radio buttons in grid
     * 
     * @param {String} type
     * @param {String} value
     * @param {Object} metaData
     * @param {Object} record
     * @param {Number} rowIndex
     * @param {Number} colIndex
     * @param {Store} store
     * @return {String}
     */
    typeRadioRenderer: function(type, value, metaData, record, rowIndex, colIndex, store) {
        if (! record.hasEmail()) {
            return '';
        }
        
        var lowerType = Ext.util.Format.lowercase(type);
        
        return this.radioTpl.apply({
            id: record.id, 
            type: lowerType,
            checked: lowerType === 'none' ? 'checked' : ''
        });
    },
    
    /**
     * called after a new set of Records has been loaded
     * 
     * @param  {Ext.data.Store} this.store
     * @param  {Array}          loaded records
     * @param  {Array}          load options
     * @return {Void}
     */
    onContactStoreLoad: function(store, records, options) {
        Ext.each(records, function(record) {
            Ext.each(['to', 'cc', 'bcc'], function(type) {
                if (this.messageRecord.data[type].indexOf(Tine.Felamimail.getEmailStringFromContact(record)) !== -1) {
                    this.setTypeRadio(record, type);
                }
            }, this);
        }, this);
    },
    
    /**
     * cell click handler -> update recipients in record
     * 
     * @param {Grid} grid
     * @param {Number} row
     * @param {Number} col
     * @param {Event} e
     */
    onCellClick: function(grid, row, col, e) {
        var contact = this.store.getAt(row),
            typeToSet = this.grid.getColumnModel().getDataIndex(col)
            
        if (! contact.hasEmail() && typeToSet !== 'none') {
            this.setTypeRadio(contact, 'none');
        } else {
            this.updateRecipients(contact, typeToSet);
        }
    },
    
    /**
     * update recipient
     * 
     * @param {Tine.Addressbook.Model.Contact} contact
     * @param {String} typeToSet
     */
    updateRecipients: function(contact, typeToSet) {
        var email = Tine.Felamimail.getEmailStringFromContact(contact),
            found = false;
            
        Ext.each(['to', 'cc', 'bcc'], function(type) {
            if (this.messageRecord.data[type].indexOf(email) !== -1) {
                if (type !== typeToSet) {
                    this.messageRecord.data[type].remove(email);
                } else {
                    found = true;
                }
            }
        }, this);
        
        if (! found && typeToSet !== 'none') {
            this.messageRecord.data[typeToSet].push(email);
        }
    },
    
    /**
     * update type radio buttons dom
     * 
     * @param {Array} records of type Tine.Addressbook.Model.Contact
     * @param {String} type
     */
    setTypeRadio: function(records, type) {
        var rs = [].concat(records);
        
        Ext.each(rs, function(r) {
            if (r.hasEmail() || type === 'none') {
                Ext.select('input[name=' + this.id + '_' + r.id + ']', this.grid.el).each(function(el) {
                    el.dom.checked = type === el.dom.value;
                });
                this.updateRecipients(r, type);
            }
        }, this);
    },
    
    /**
     * Return CSS class to apply to rows depending upon email set or not
     * 
     * @param {Tine.Addressbook.Model.Contact} record
     * @param {Integer} index
     * @return {String}
     */
    getViewRowClass: function(record, index) {
        var className = '';
        
        if (! record.hasEmail()) {
            className = 'felamimail-no-email';
        }
        
        return className;
    },
    
    /**
     * @private
     */
    initActions: function() {
        this.actions_addAsTo = new Ext.Action({
            requiredGrant: 'readGrant',
            text: this.app.i18n._('Add as "To"'),
            disabled: true,
            iconCls: 'action_add',
            actionUpdater: this.updateRecipientActions,
            handler: this.onAddContact.createDelegate(this, ['to']),
            allowMultiple: true,
            scope: this
        });

        this.actions_addAsCc = new Ext.Action({
            requiredGrant: 'readGrant',
            text: this.app.i18n._('Add as "Cc"'),
            disabled: true,
            iconCls: 'action_add',
            actionUpdater: this.updateRecipientActions,
            handler: this.onAddContact.createDelegate(this, ['cc']),
            allowMultiple: true,
            scope: this
        });

        this.actions_addAsBcc = new Ext.Action({
            requiredGrant: 'readGrant',
            text: this.app.i18n._('Add as "Bcc"'),
            disabled: true,
            iconCls: 'action_add',
            actionUpdater: this.updateRecipientActions,
            handler: this.onAddContact.createDelegate(this, ['bcc']),
            allowMultiple: true,
            scope: this
        });
        
        this.actions_setToNone = new Ext.Action({
            requiredGrant: 'readGrant',
            text: this.app.i18n._('Remove from recipients'),
            disabled: true,
            iconCls: 'action_delete',
            actionUpdater: this.updateRecipientActions,
            handler: this.onAddContact.createDelegate(this, ['none']),
            allowMultiple: true,
            scope: this
        });
        
        //register actions in updater
        this.actionUpdater.addActions([
            this.actions_addAsTo,
            this.actions_addAsCc,
            this.actions_addAsBcc,
            this.actions_setToNone
        ]);
    },
    
    /**
     * updates context menu
     * 
     * @param {Ext.Action} action
     * @param {Object} grants grants sum of grants
     * @param {Object} records
     */
    updateRecipientActions: function(action, grants, records) {
        if (records.length > 0) {
            var emptyEmails = true;
            for (var i=0; i < records.length; i++) {
                if (records[i].hasEmail()) {
                    emptyEmails = false;
                    break;
                }
            }
            
            action.setDisabled(emptyEmails);
        } else {
            action.setDisabled(true);
        }
    },
    
    /**
     * on add contact -> fires addcontacts event and passes rows + type
     * 
     * @param {String} type
     */
    onAddContact: function(type) {
        var sm = this.grid.getSelectionModel(),
            selectedRows = sm.getSelections();
            
        this.setTypeRadio(selectedRows, type);

        // search contacts if all pages are selected (filter select)
        if (sm.isFilterSelect) {
            this['AddressLoadMask'] = new Ext.LoadMask(Ext.getBody(), {msg: this.app.i18n._('Loading Mail Addresses')});
            this['AddressLoadMask'].show();
            
            var contact = null;
            Tine.Addressbook.searchContacts(sm.getSelectionFilter(), null, function(response) {
                Ext.each(response.results, function(contactData) {
                    contact = new Tine.Addressbook.Model.Contact(contactData);
                    this.updateRecipients(contact, type);
                }, this);
                
                this['AddressLoadMask'].hide();
            }.createDelegate(this));
        }
    },
    
    /**
     * row doubleclick handler
     * 
     * @param {} grid
     * @param {} row
     * @param {} e
     */
    onRowDblClick: function(grid, row, e) {
        this.onAddContact('to');
    }, 
    
    /**
     * returns rows context menu
     * 
     * @return {Ext.menu.Menu}
     */
    getContextMenu: function() {
        if (! this.contextMenu) {
            var items = [
                this.actions_addAsTo,
                this.actions_addAsCc,
                this.actions_addAsBcc,
                this.actions_setToNone
            ];
            this.contextMenu = new Ext.menu.Menu({items: items});
        }
        return this.contextMenu;
    }
});

Ext.reg('felamimailcontactgrid', Tine.Felamimail.ContactGridPanel);
/*
 * Tine 2.0
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2011 Metaways Infosystems GmbH (http://www.metaways.de)
 */
Ext.ns('Tine.Felamimail');

/**
 * @namespace   Tine.Felamimail
 * @class       Tine.Felamimail.RecipientPickerFavoritePanel
 * @extends     Ext.tree.TreePanel
 * 
 * <p>PersistentFilter Picker Panel</p>
 * 
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * 
 * @param       {Object} config
 * @constructor
 * Create a new Tine.Felamimail.RecipientPickerFavoritePanel
 */
Tine.Felamimail.RecipientPickerFavoritePanel = Ext.extend(Tine.widgets.persistentfilter.PickerPanel, {
    
    collapsible: true,
    baseCls: 'ux-arrowcollapse',
    animCollapse: true,
    titleCollapse:true,
    draggable : true,
    autoScroll: false,
    
    /**
     * no context menu
     * @type Function
     */
    onContextMenu: Ext.emptyFn,
    
    /**
     * @private
     */
    initComponent: function() {
        this.title = this.app.i18n._('Recipient filter');
        
        this.store = new Ext.data.ArrayStore({
            fields: Tine.widgets.persistentfilter.model.PersistentFilter.getFieldDefinitions(),
            sortInfo: {field: 'name', direction: 'ASC'}
        });
        
        var label = '';
        Ext.each(['all', 'to', 'cc', 'bcc'], function(field) {
            switch (field) {
                case 'all':
                    label = this.app.i18n._('All recipients');
                    break;
                default:
                    label = String.format(this.app.i18n._('"{0}" recipients'), field);
                    break;
            }
            this.store.add([new Tine.widgets.persistentfilter.model.PersistentFilter({
                filters: field,
                name: label,
                model: 'Addressbook_Model_Contact',
                application_id: this.app.id,
                id: Ext.id()
            })]);
        }, this);

        
        this.filterNode = new Ext.tree.AsyncTreeNode({
            id: '_recipientFilter',
            leaf: false,
            expanded: true
        });
        
        Tine.Felamimail.RecipientPickerFavoritePanel.superclass.initComponent.call(this);
    },
    
    /**
     * load grid from saved filter
     * 
     * -> overwritten to allow to dynamically update email filter
     * 
     *  @param {Tine.widgets.persistentfilter.model.PersistentFilter} persistentFilter
     */
    onFilterSelect: function(persistentFilter) {
        var emailRecipients = [];

        switch (persistentFilter.get('filters')) {
            case 'all':
                Ext.each(['to', 'cc', 'bcc'], function(field) {
                    emailRecipients = emailRecipients.concat(this.grid.messageRecord.data[field]);
                }, this);
                break;
            default:
                emailRecipients = this.grid.messageRecord.data[persistentFilter.get('filters')];
                break;
        }
        
        var filterValue = [], emailRegExp = /<([^>]*)/, filter = [{field: 'container_id', operator: 'in', value: []}];
        Ext.each(emailRecipients, function(email) {
            emailRegExp.exec(email);
            if (RegExp.$1 != '') {
                filterValue.push(RegExp.$1)
            }
        }, this);
        if (filterValue.length > 0) {
            filter = [{field: 'email_query', operator: 'in', value: filterValue}];
        }

        var updatedPersistentFilter = new Tine.widgets.persistentfilter.model.PersistentFilter({
            filters: filter,
            name: persistentFilter.get('name'),
            model: 'Addressbook_Model_Contact',
            application_id: this.app.id,
            id: Ext.id()
        });

        Tine.Felamimail.RecipientPickerFavoritePanel.superclass.onFilterSelect.call(this, updatedPersistentFilter);
    }
});
/*
 * Tine 2.0
 * 
 * @package     Felamimail
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2010-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 *
 */
 
Ext.namespace('Tine.Felamimail');

/**
 * @namespace   Tine.Felamimail
 * @class       Tine.Felamimail.RecipientPickerDialog
 * @extends     Tine.widgets.dialog.EditDialog
 * 
 * <p>Message Compose Dialog</p>
 * <p>This dialog is for searching contacts in the addressbook and adding them to the recipient list in the email compose dialog.</p>
 * <p>
 * </p>
 * 
 * @author      Philipp Schuele <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2010-2011 Metaways Infosystems GmbH (http://www.metaways.de)
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * 
 * @param       {Object} config
 * @constructor
 * Create a new RecipientPickerDialog
 */
 Tine.Felamimail.RecipientPickerDialog = Ext.extend(Tine.widgets.dialog.EditDialog, {
    
    /**
     * @private
     */
    windowNamePrefix: 'RecipientPickerWindow_',
    appName: 'Felamimail',
    recordClass: Tine.Felamimail.Model.Message,
    recordProxy: Tine.Felamimail.messageBackend,
    loadRecord: false,
    evalGrants: false,
    mode: 'local',
    
    bodyStyle:'padding:0px',
    
    /**
     * overwrite update toolbars function (we don't have record grants)
     * @private
     */
    updateToolbars: Ext.emptyFn,
    
    /**
     * @private
     */
    onRecordLoad: function() {
        // interrupt process flow till dialog is rendered
        if (! this.rendered) {
            this.onRecordLoad.defer(250, this);
            return;
        }
        
        var subject = (this.record.get('subject') != '') ? this.record.get('subject') : this.app.i18n._('(new message)');
        this.window.setTitle(String.format(this.app.i18n._('Select recipients for "{0}"'), subject));
        
        this.loadMask.hide();
    },

    /**
     * returns dialog
     * 
     * NOTE: when this method gets called, all initialisation is done.
     * 
     * @return {Object}
     * @private
     */
    getFormItems: function() {
        var adbApp = Tine.Tinebase.appMgr.get('Addressbook');
        
        this.treePanel = new Tine.widgets.container.TreePanel({
            allowMultiSelection: true,
            region: 'west',
            filterMode: 'filterToolbar',
            recordClass: Tine.Addressbook.Model.Contact,
            app: adbApp,
            width: 200,
            minSize: 100,
            maxSize: 300,
            border: false,
            enableDrop: false
        });
        
        this.contactGrid = new Tine.Felamimail.ContactGridPanel({
            region: 'center',
            messageRecord: this.record,
            app: adbApp,
            plugins: [this.treePanel.getFilterPlugin()]
        });
        
        this.westPanel = new Tine.widgets.mainscreen.WestPanel({
            app: adbApp,
            hasFavoritesPanel: true,
            ContactTreePanel: this.treePanel,
            ContactFilterPanel: new Tine.widgets.persistentfilter.PickerPanel({
                filter: [{field: 'model', operator: 'equals', value: 'Addressbook_Model_ContactFilter'}],
                app: adbApp,
                grid: this.contactGrid
            }),
            additionalItems: [ new Tine.Felamimail.RecipientPickerFavoritePanel({
                app: this.app,
                grid: this.contactGrid
            })]
        });
        
        return {
            border: false,
            layout: 'border',
            items: [{
                cls: 'tine-mainscreen-centerpanel-west',
                region: 'west',
                stateful: false,
                layout: 'border',
                split: true,
                width: 200,
                minSize: 100,
                maxSize: 300,
                border: false,
                collapsible: true,
                collapseMode: 'mini',
                header: false,
                items: [{
                    border: false,
                    region: 'center',
                    items: [ this.westPanel ]
                }]
            }, this.contactGrid]
        };
    }
});

/**
 * Felamimail Edit Popup
 * 
 * @param   {Object} config
 * @return  {Ext.ux.Window}
 */
Tine.Felamimail.RecipientPickerDialog.openWindow = function (config) {
    var window = Tine.WindowFactory.getWindow({
        width: 1000,
        height: 600,
        name: Tine.Felamimail.RecipientPickerDialog.prototype.windowNamePrefix + Ext.id(),
        contentPanelConstructor: 'Tine.Felamimail.RecipientPickerDialog',
        contentPanelConstructorConfig: config
    });
    return window;
};
/*
 * Tine 2.0
 * 
 * @license     http://www.gnu.org/licenses/agpl.html AGPL Version 3
 * @author      Philipp Schüle <p.schuele@metaways.de>
 * @copyright   Copyright (c) 2010-2012 Metaways Infosystems GmbH (http://www.metaways.de)
 * 
 */
Ext.ns('Tine.Felamimail');

/**
 * @namespace   Tine.widgets.container
 * @class       Tine.Felamimail.FolderFilterModel
 * @extends     Tine.widgets.grid.FilterModel
 * 
 * @author      Philipp Schuele <p.schuele@metaways.de>
 */
Tine.Felamimail.FolderFilterModel = Ext.extend(Tine.widgets.grid.PickerFilter, {

    /**
     * @cfg 
     */
    operators: ['in', 'notin'],
    field: 'path',
    
    /**
     * @private
     */
    initComponent: function() {
        this.label = this.app.i18n._('Folder');
        
        this.multiselectFieldConfig = {
            labelField: 'path',
            selectionWidget: new Tine.Felamimail.FolderSelectTriggerField({
                allAccounts: true
            }),
            recordClass: Tine.Felamimail.Model.Folder,
            valueStore: this.app.getFolderStore(),
            
            /**
             * functions
             */
            labelRenderer: Tine.Felamimail.GridPanel.prototype.accountAndFolderRenderer.createDelegate(this),
            initSelectionWidget: function() {
                this.selectionWidget.onSelectFolder = this.addRecord.createDelegate(this);
            },
            isSelectionVisible: function() {
                return this.selectionWidget.selectPanel && ! this.selectionWidget.selectPanel.isDestroyed        
            },
            getRecordText: function(value) {
                var path = (Ext.isString(value)) ? value : (value.path) ? value.path : '/' + value.id,
                    index = this.valueStore.findExact('path', path),
                    record = this.valueStore.getAt(index),
                    text = null;
                
                if (! record) {
                    // try account
                    var accountId = path.substr(1, 40);
                    record = this.app.getAccountStore().getById(accountId);
                }
                if (record) {
                    this.currentValue.push(path);
                    // always copy/clone record because it can't exist in 2 different stores
                    this.store.add(record.copy());
                    text = this.labelRenderer(record.id, {}, record);
                } else {
                    text = value;
                    this.currentValue.push(value);
                }
                
                return text;
            }
        };

        Tine.Felamimail.FolderFilterModel.superclass.initComponent.call(this);
    }
});

Tine.widgets.grid.FilterToolbar.FILTERS['tine.felamimail.folder.filtermodel'] = Tine.Felamimail.FolderFilterModel;
