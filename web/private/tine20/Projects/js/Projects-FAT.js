/*
 * Tine 2.0 - Projects 
 * Copyright (c) 2011 Metaways Infosystems GmbH (http://www.metaways.de)http://www.gnu.org/licenses/agpl.html AGPL Version 3
 */
Ext.ns("Tine.Projects");Tine.Projects.ProjectAttendeeFilter=Ext.extend(Tine.widgets.grid.ForeignRecordFilter,{foreignRecordClass:Tine.Addressbook.Model.Contact,ownField:"contact",initComponent:function(){this.label=this.app.i18n._("Attendee");Tine.Projects.ProjectAttendeeFilter.superclass.initComponent.call(this)},getSubFilters:function(){var a=new Tine.Tinebase.widgets.keyfield.Filter({label:this.app.i18n._("Attendee Role"),field:"relation_type",app:this.app,keyfieldName:"projectAttendeeRole"});return[a]}});Tine.widgets.grid.FilterToolbar.FILTERS["tine.projects.attendee"]=Tine.Projects.ProjectAttendeeFilter;Ext.ns("Tine.Projects.Model");Tine.Projects.Model.Project=Tine.Tinebase.data.Record.create(Tine.Tinebase.Model.genericFields.concat([{name:"id"},{name:"title"},{name:"number"},{name:"description"},{name:"status"},{name:"relations"},{name:"notes"},{name:"tags"}]),{appName:"Projects",modelName:"Project",idProperty:"id",titleProperty:"title",recordName:"Project",recordsName:"Projects",containerProperty:"container_id",containerName:"Project list",containersName:"Project lists"});Tine.Projects.Model.Project.getDefaultData=function(){var a=Tine.Tinebase.appMgr.get("Projects");var b=Tine.Projects.registry.get("defaultContainer");return{container_id:a.getMainScreen().getWestPanel().getContainerTreePanel().getDefaultContainer(),status:"IN-PROCESS"}};Tine.Projects.Model.Project.getFilterModel=function(){var a=Tine.Tinebase.appMgr.get("Projects");return[{label:_("Quick search"),field:"query",operators:["contains"]},{label:a.i18n._("Title"),field:"title"},{label:a.i18n._("Number"),field:"number"},{label:a.i18n._("Description"),field:"description"},{label:a.i18n._("Status"),field:"status",filtertype:"tine.widget.keyfield.filter",app:a,keyfieldName:"projectStatus"},{filtertype:"tinebase.tag",app:a},{filtertype:"tine.widget.container.filtermodel",app:a,recordClass:Tine.Projects.Model.Project},{filtertype:"tine.projects.attendee",app:a},{label:_("Last Modified Time"),field:"last_modified_time",valueType:"date"},{label:_("Last Modified By"),field:"last_modified_by",valueType:"user"},{label:_("Creation Time"),field:"creation_time",valueType:"date"},{label:_("Created By"),field:"created_by",valueType:"user"}]};Tine.Projects.recordBackend=new Tine.Tinebase.data.RecordProxy({appName:"Projects",modelName:"Project",recordClass:Tine.Projects.Model.Project});Ext.ns("Tine.Projects");Tine.Projects.Application=Ext.extend(Tine.Tinebase.Application,{getTitle:function(){return this.i18n.gettext("Projects")},init:function(){new Tine.Projects.AddressbookGridPanelHook({app:this})}});Tine.Projects.MainScreen=Ext.extend(Tine.widgets.MainScreen,{activeContentType:"Project"});Tine.Projects.ProjectTreePanel=Ext.extend(Tine.widgets.container.TreePanel,{id:"Projects_Tree",filterMode:"filterToolbar",recordClass:Tine.Projects.Model.Project});Tine.Projects.ProjectFilterPanel=function(a){Ext.apply(this,a);Tine.Projects.ProjectFilterPanel.superclass.constructor.call(this)};Ext.extend(Tine.Projects.ProjectFilterPanel,Tine.widgets.persistentfilter.PickerPanel,{filter:[{field:"model",operator:"equals",value:"Projects_Model_ProjectFilter"}]});Ext.ns("Tine.Projects");Tine.Projects.ProjectGridPanel=Ext.extend(Tine.widgets.grid.GridPanel,{recordClass:Tine.Projects.Model.Project,evalGrants:true,ftbConfig:null,defaultSortInfo:{field:"creation_time",direction:"DESC"},gridConfig:{autoExpandColumn:"title"},initComponent:function(){this.recordProxy=Tine.Projects.recordBackend;this.gridConfig.cm=this.getColumnModel();this.filterToolbar=this.filterToolbar||this.getFilterToolbar(this.ftbConfig);this.plugins=this.plugins||[];this.plugins.push(this.filterToolbar);Tine.Projects.ProjectGridPanel.superclass.initComponent.call(this)},getColumnModel:function(){return new Ext.grid.ColumnModel({defaults:{sortable:true,resizable:true},columns:[{id:"tags",header:this.app.i18n._("Tags"),width:40,dataIndex:"tags",sortable:false,renderer:Tine.Tinebase.common.tagsRenderer},{id:"number",header:this.app.i18n._("Number"),width:100,sortable:true,dataIndex:"number",hidden:true},{id:"title",header:this.app.i18n._("Title"),width:350,sortable:true,dataIndex:"title"},{id:"status",header:this.app.i18n._("Status"),width:150,sortable:true,dataIndex:"status",renderer:Tine.Tinebase.widgets.keyfield.Renderer.get("Projects","projectStatus")}].concat(this.getModlogColumns())})},statusRenderer:function(a){return this.app.i18n._hidden(a)}});Ext.ns("Tine.Projects");Tine.Projects.AddressbookGridPanelHook=function(a){Tine.log.info("initialising projects addressbook hooks");Ext.apply(this,a);var b=this.app.i18n.n_hidden(Tine.Projects.Model.Project.prototype.recordName,Tine.Projects.Model.Project.prototype.recordsName,1);this.addProjectAction=new Ext.Action({requiredGrant:"readGrant",text:b,iconCls:this.app.getIconCls(),scope:this,handler:this.onUpdateProject,listeners:{scope:this,render:this.onRender}});this.newProjectAction=new Ext.Action({requiredGrant:"readGrant",text:b,iconCls:this.app.getIconCls(),scope:this,handler:this.onAddProject,listeners:{scope:this,render:this.onRender}});Ext.ux.ItemRegistry.registerItem("Addressbook-GridPanel-ContextMenu-Add",this.addProjectAction,90);Ext.ux.ItemRegistry.registerItem("Addressbook-GridPanel-ContextMenu-New",this.newProjectAction,90)};Ext.apply(Tine.Projects.AddressbookGridPanelHook.prototype,{app:null,handleProjectsAction:null,ContactGridPanel:null,projectsMenu:null,getContactGridPanel:function(){if(!this.ContactGridPanel){this.ContactGridPanel=Tine.Tinebase.appMgr.get("Addressbook").getMainScreen().getCenterPanel()}return this.ContactGridPanel},onAddProject:function(){var b=this.app.getMainScreen(),d=b.getCenterPanel(),c=this.getFilter(b),e=this.getContactGridPanel().selectionModel;if(!c){var a=this.getSelectionsAsArray()}else{var a=true}d.onEditInNewWindow.call(d,"add",null,[{ptype:"addrelations_edit_dialog",selectionFilter:c,addRelations:a,callingApp:"Addressbook",callingModel:"Contact"}])},onUpdateProject:function(){var b=this.app.getMainScreen(),e=b.getCenterPanel(),c=this.getFilter(b),f=this.getContactGridPanel().selectionModel;if(!c){var a=this.getSelectionsAsArray(),d=this.getSelectionsAsArray().length}else{var a=true,d=f.store.totalLength}Tine.Projects.AddToProjectPanel.openWindow({count:d,selectionFilter:c,addRelations:a,callingApp:"Addressbook",callingModel:"Contact"})},getFilter:function(b){var d=this.getContactGridPanel().selectionModel,a=this.getSelectionsAsArray(),c=null;if(d.isFilterSelect){var c=d.getSelectionFilter()}return c},getSelectionsAsArray:function(){var b=this.getContactGridPanel().grid.getSelectionModel().getSelections(),a=[];Ext.each(b,function(c){if(c.data){a.push(c.data)}});return a},onRender:function(){var b=this.getContactGridPanel().actionUpdater,a=b.actions;if(a.indexOf(this.addEventAction)<0){b.addActions([this.addEventAction])}if(a.indexOf(this.newEventAction)<0){b.addActions([this.newEventAction])}}});Ext.ns("Tine.Projects");Tine.Projects.ProjectEditDialog=Ext.extend(Tine.widgets.dialog.EditDialog,{windowNamePrefix:"ProjectEditWindow_",appName:"Projects",recordClass:Tine.Projects.Model.Project,recordProxy:Tine.Projects.recordBackend,tbarItems:[{xtype:"widget-activitiesaddbutton"}],evalGrants:true,showContainerSelector:true,hideRelationsPanel:true,updateToolbars:function(){},onAfterRecordLoad:function(){Tine.Projects.ProjectEditDialog.superclass.onAfterRecordLoad.call(this);this.contactLinkPanel.onRecordLoad(this.record)},onRecordUpdate:function(){Tine.Projects.ProjectEditDialog.superclass.onRecordUpdate.call(this);this.record.set("relations",this.contactLinkPanel.getData())},getFormItems:function(){this.contactLinkPanel=new Tine.widgets.grid.LinkGridPanel({app:this.app,searchRecordClass:Tine.Addressbook.Model.Contact,title:this.app.i18n._("Attendee"),typeColumnHeader:this.app.i18n._("Role"),searchComboClass:Tine.Addressbook.SearchCombo,searchComboConfig:{relationDefaults:{type:this.app.getRegistry().get("config")["projectAttendeeRole"].definition["default"],own_model:"Projects_Model_Project",related_model:"Addressbook_Model_Contact",own_degree:"sibling",related_backend:"Sql"}},relationTypesKeyfieldName:"projectAttendeeRole"});return{xtype:"tabpanel",border:false,plain:true,plugins:[{ptype:"ux.tabpanelkeyplugin"}],activeTab:0,border:false,items:[{title:this.app.i18n._("Project"),autoScroll:true,border:false,frame:true,layout:"border",items:[{region:"center",layout:"hfit",border:false,items:[{xtype:"fieldset",layout:"hfit",autoHeight:true,title:this.app.i18n._("Project"),items:[{xtype:"columnform",labelAlign:"top",formDefaults:{xtype:"textfield",anchor:"100%",labelSeparator:"",columnWidth:0.333},items:[[{columnWidth:1,fieldLabel:this.app.i18n._("Title"),name:"title",allowBlank:false}],[{columnWidth:0.5,fieldLabel:this.app.i18n._("Number"),name:"number"},new Tine.Tinebase.widgets.keyfield.ComboBox({columnWidth:0.5,app:"Projects",keyFieldName:"projectStatus",fieldLabel:this.app.i18n._("Status"),name:"status"})]]}]},{xtype:"tabpanel",deferredRender:false,activeTab:0,border:false,height:250,form:true,items:[this.contactLinkPanel]}]},{layout:"accordion",animate:true,region:"east",width:210,split:true,collapsible:true,collapseMode:"mini",header:false,margins:"0 5 0 5",border:true,items:[new Ext.Panel({title:this.app.i18n._("Description"),iconCls:"descriptionIcon",layout:"form",labelAlign:"top",border:false,items:[{style:"margin-top: -4px; border 0px;",labelSeparator:"",xtype:"textarea",name:"description",hideLabel:true,grow:false,preventScrollbars:false,anchor:"100% 100%",emptyText:this.app.i18n._("Enter description"),requiredGrant:"editGrant"}]}),new Tine.widgets.activities.ActivitiesPanel({app:"Projects",showAddNoteForm:false,border:false,bodyStyle:"border:1px solid #B5B8C8;"}),new Tine.widgets.tags.TagPanel({app:"Projects",border:false,bodyStyle:"border:1px solid #B5B8C8;"})]}]},new Tine.widgets.activities.ActivitiesTabPanel({app:this.appName,record_id:this.record.id,record_model:this.appName+"_Model_"+this.recordClass.getMeta("modelName")})]}}});Tine.Projects.ProjectEditDialog.openWindow=function(a){var c=(a.record&&a.record.id)?a.record.id:0;var b=Tine.WindowFactory.getWindow({width:800,height:470,name:Tine.Projects.ProjectEditDialog.prototype.windowNamePrefix+c,contentPanelConstructor:"Tine.Projects.ProjectEditDialog",contentPanelConstructorConfig:a});return b};Ext.ns("Tine.Projects");Tine.Projects.SearchCombo=Ext.extend(Tine.Tinebase.widgets.form.RecordPickerComboBox,{allowBlank:false,itemSelector:"div.search-item",minListWidth:200,initComponent:function(){this.recordClass=Tine.Projects.Model.Project;this.recordProxy=Tine.Projects.recordBackend;this.initTemplate();Tine.Projects.SearchCombo.superclass.initComponent.call(this)},onBeforeQuery:function(a){Tine.Projects.SearchCombo.superclass.onBeforeQuery.apply(this,arguments)},initTemplate:function(){if(!this.tpl){this.tpl=new Ext.XTemplate('<tpl for="."><div class="search-item">','<table cellspacing="0" cellpadding="2" border="0" style="font-size: 11px;" width="100%">',"<tr>",'<td style="height:16px">{[this.encode(values)]}</td>',"</tr>","</table>","</div></tpl>",{encode:function(a){var b="<b>"+Ext.util.Format.htmlEncode(a.title)+"</b>";if(a.number){b+=" ("+a.number+")"}return b}})}},getValue:function(){return Tine.Projects.SearchCombo.superclass.getValue.call(this)},setValue:function(a){return Tine.Projects.SearchCombo.superclass.setValue.call(this,a)}});Tine.widgets.form.RecordPickerManager.register("Projects","Project",Tine.Projects.SearchCombo);Ext.ns("Tine.Projects");Tine.Projects.AddToProjectPanel=Ext.extend(Tine.widgets.dialog.AddToRecordPanel,{appName:"Projects",recordClass:Tine.Projects.Model.Project,callingApp:"Addressbook",callingModel:"Contact",isValid:function(){var a=true;if(this.searchBox.getValue()==""){this.searchBox.markInvalid(this.app.i18n._("Please choose the Project to add the contacts to"));a=false}if(this.chooseRoleBox.getValue()==""){this.chooseRoleBox.markInvalid(this.app.i18n._("Please select the attenders' role"));a=false}return a},getRelationConfig:function(){var a={type:this.chooseRoleBox.getValue()?this.chooseRoleBox.getValue():"COWORKER"};return a},getFormItems:function(){return{border:false,frame:false,layout:"border",items:[{region:"center",border:false,frame:false,layout:{align:"stretch",type:"vbox"},items:[{layout:"form",margins:"10px 10px",border:false,frame:false,items:[Tine.widgets.form.RecordPickerManager.get("Projects","Project",{fieldLabel:this.app.i18n._("Select Project"),anchor:"100% 100%",ref:"../../../searchBox"}),{fieldLabel:this.app.i18n._("Role"),emptyText:this.app.i18n._("Select Role"),anchor:"100% 100%",xtype:"widget-keyfieldcombo",app:"Projects",value:"COWORKER",keyFieldName:"projectAttendeeRole",ref:"../../../chooseRoleBox"}]}]}]}}});Tine.Projects.AddToProjectPanel.openWindow=function(a){var b=Tine.WindowFactory.getWindow({modal:true,title:String.format(Tine.Tinebase.appMgr.get("Projects").i18n._("Adding {0} Participants to project"),a.count),width:250,height:150,contentPanelConstructor:"Tine.Projects.AddToProjectPanel",contentPanelConstructorConfig:a});return b};