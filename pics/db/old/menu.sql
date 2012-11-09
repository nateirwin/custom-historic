#
# Table structure for table `tiki_menu_options`
#
# Creation: Jul 03, 2003 at 07:42 PM
# Last update: Nov 21, 2003 at 07:05 AM
#
                                                                                                                                                                                                         
DROP TABLE IF EXISTS tiki_menu_options;
CREATE TABLE tiki_menu_options (
  optionId int(8) NOT NULL auto_increment,
  menuId int(8) default NULL,
  type char(1) default NULL,
  name varchar(200) default NULL,
  url varchar(255) default NULL,
  position int(4) default NULL,
  section varchar(255) default NULL,
  perm varchar(255) default NULL,
  groupname varchar(255) default NULL,
  PRIMARY KEY  (optionId)
) TYPE=MyISAM AUTO_INCREMENT=1 ;
# --------------------------------------------------------
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Home','tiki-index.php',10,'','','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Chat','tiki-chat.php',15,'feature_chat','tiki_p_chat','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Contact us','tiki-contact.php',20,'feature_contact','','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Stats','tiki-stats.php',23,'feature_stats','tiki_p_view_stats','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Categories','tiki-browse_categories.php',25,'feature_categories','tiki_p_view_categories','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Games','tiki-games.php',30,'feature_games','tiki_p_play_games','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Calendar','tiki-calendar.php',35,'feature_calendar','tiki_p_view_calendar','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','(debug)',"javascript:toggle('debugconsole')",40,'feature_debug_console','tiki_p_admin','');
                                                                                                                                                                                                         
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'s','MyTiki (click!)','tiki-my_tiki.php',50,'','','Registered');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Preferences','tiki-user_preferences.php',55,'feature_userPreferences','','Registered');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Messages','messu-mailbox.php',60,'feature_messages','tiki_p_messages','Registered');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Tasks','tiki-user_tasks.php',65,'feature_tasks','tiki_p_tasks','Registered');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Bookmarks','tiki-user_bookmarks.php',70,'feature_user_bookmarks','tiki_p_create_bookmarks','Registered');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Modules','tiki-user_assigned_modules.php',75,'user_assigned_modules','tiki_p_configure_modules','Registered');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Newsreader','tiki-newsreader_servers.php',80,'feature_newsreader','tiki_p_newsreader','Registered');INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Webmail','tiki-webmail.php',85,'feature_webmail','tiki_p_use_webmail','Registered');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Notepad','tiki-notepad_list.php',90,'feature_notepad','tiki_p_notepad','Registered');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','My files','tiki-userfiles.php',95,'feature_userfiles','tiki_p_userfiles','Registered');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','User menu','tiki-usermenu.php',100,'feature_usermenu','','Registered');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','My watches','tiki-user_watches.php',110,'feature_user_watches','','Registered');
                                                                                                                                                                                                         
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'s','Workflow','tiki-g-user_processes.php',150,'feature_workflow','tiki_p_use_workflow','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Admin processes','tiki-g-admin_processes.php',155,'feature_workflow','tiki_p_admin_workflow','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Monitor processes','tiki-g-monitor_processes.php',160,'feature_workflow','tiki_p_admin_workflow','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Monitor activities','tiki-g-monitor_activities.php',165,'feature_workflow','tiki_p_admin_workflow','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Monitor instances','tiki-g-monitor_instances.php',170,'feature_workflow','tiki_p_admin_workflow','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','User processes','tiki-g-user_processes.php',175,'feature_workflow','tiki_p_use_workflow','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','User activities','tiki-g-user_activities.php',180,'feature_workflow','tiki_p_use_workflow','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','User instances','tiki-g-user_instances.php',185,'feature_workflow','tiki_p_use_workflow','');
                                                                                                                                                                                                         
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'s','Wiki','tiki-index.php',200,'feature_wiki','tiki_p_view','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Wiki Home','tiki-index.php',202,'feature_wiki','tiki_p_view','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Last Changes','tiki-lastchanges.php',205,'feature_wiki,feature_lastChanges','tiki_p_view','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Dump','dump/new.tar',210,'feature_wiki,feature_dump','tiki_p_view','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Rankings','tiki-wiki_rankings.php',215,'feature_wiki,feature_wiki_rankings','tiki_p_view','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','List pages','tiki-listpages.php',220,'feature_wiki,feature_listPages','tiki_p_view','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Orphan pages','tiki-orphan_pages.php',225,'feature_wiki,feature_listPages','tiki_p_view','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Sandbox','tiki-editpage.php?page=sandbox',230,'feature_wiki,feature_sandbox','tiki_p_view','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Print','tiki-print_pages.php',235,'feature_wiki,feature_wiki_multiprint','tiki_p_view','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Send pages','tiki-send_objects.php',240,'feature_wiki,feature_comm','tiki_p_view,tiki_p_send_pages','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Received pages','tiki-received_pages.php',245,'feature_wiki,feature_comm','tiki_p_view,tiki_p_admin_received_pages','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Structures','tiki-admin_structures.php',250,'feature_wiki','tiki_p_edit_structures','');
                                                                                                                                                                                                         
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'s','Image Galleries','tiki-galleries.php',300,'feature_galleries','tiki_p_view_image_gallery','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Galleries','tiki-galleries.php',305,'feature_galleries','tiki_p_view_image_gallery','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Rankings','tiki-galleries_rankings.php',310,'feature_galleries,feature_gal_rankings','tiki_p_view_image_gallery','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Upload image','tiki-upload_image.php',315,'feature_galleries','tiki_p_upload_images','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','System gallery','tiki-list_gallery.php?galleryId=0',320,'feature_galleries','tiki_p_admin_galleries','');
                                                                                                                                                                                                         
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'s','Articles','tiki-view_articles.php',350,'feature_articles','tiki_p_read_article','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Articles home','tiki-view_articles.php',355,'feature_articles','tiki_p_read_article','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','List articles','tiki-list_articles.php',360,'feature_articles','tiki_p_read_article','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Rankings','tiki-cms_rankings.php',365,'feature_articles,feature_cms_ranking','tiki_p_read_article','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Submit article','tiki-edit_submission.php',370,'feature_articles,feature_submissions','tiki_p_read_article,tiki_p_submit_article','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','View submissions','tiki-list_submissions.php',375,'feature_articles,feature_submissions','tiki_p_read_article,tiki_p_submit_article','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','View submissions','tiki-list_submissions.php',375,'feature_articles,feature_submissions','tiki_p_read_article,tiki_p_approve_submission','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','View submissions','tiki-list_submissions.php',375,'feature_articles,feature_submissions','tiki_p_read_article,tiki_p_remove_submission','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Edit article','tiki-edit_article.php',380,'feature_articles','tiki_p_read_article,tiki_p_edit_article','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Send articles','tiki-send_objects.php',385,'feature_articles,feature_comm','tiki_p_read_article,tiki_p_send_articles','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Received articles','tiki-send_objects.php',385,'feature_articles,feature_comm','tiki_p_read_article,tiki_p_send_articles','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Admin topics','tiki-admin_topics.php',390,'feature_articles','tiki_p_read_article,tiki_p_admin_cms','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Admin types','tiki-articles_types.php',395,'feature_articles','tiki_p_read_article,tiki_p_admin_cms','');
                                                                                                                                                                                                         
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'s','Blogs','tiki-list_blogs.php',450,'feature_blogs','tiki_p_read_blog','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','List blogs','tiki-list_blogs.php',455,'feature_blogs','tiki_p_read_blog','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Rankings','tiki-blogs_rankings.php',460,'feature_blogs,feature_blog_rankings','tiki_p_read_blog','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Create/Edit blog','tiki-edit_blog.php',465,'feature_blogs','tiki_p_read_blog,tiki_p_create_blogs','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Post','tiki-blog_post.php',470,'feature_blogs','tiki_p_read_blog,tiki_p_blog_post','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Admin posts','tiki-list_posts.php',475,'feature_blogs','tiki_p_read_blog,tiki_p_blog_admin','');
                                                                                                                                                                                                         
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'s','Forums','tiki-forums.php',500,'feature_forums','tiki_p_forum_read','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','List forums','tiki-forums.php',505,'feature_forums','tiki_p_forum_read','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Rankings','tiki-forum_rankings.php',510,'feature_forums,feature_forum_rankings','tiki_p_forum_read','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Admin forums','tiki-admin_forums.php',515,'feature_forums','tiki_p_forum_read,tiki_p_admin_forum','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'s','Directory','tiki-directory_browse.php',550,'feature_directory','tiki_p_view_directory','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Submit a new link','tiki-directory_add_site.php',555,'feature_directory','tiki_p_view_directory','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Browse directory','tiki-directory_browse.php',560,'feature_directory','tiki_p_view_directory','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Admin directory','tiki-directory_admin.php',565,'feature_directory','tiki_p_view_directory,tiki_p_admin_directory_cats','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Admin directory','tiki-directory_admin.php',565,'feature_directory','tiki_p_view_directory,tiki_p_admin_directory_sites','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Admin directory','tiki-directory_admin.php',565,'feature_directory','tiki_p_view_directory,tiki_p_validate_links','');
                                                                                                                                                                                                         
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'s','File Galleries','tiki-file_galleries.php',600,'feature_file_galleries','tiki_p_view_file_gallery','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','List galleries','tiki-file_galleries.php',605,'feature_file_galleries','tiki_p_view_file_gallery','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Rankings','tiki-file_galleries_rankings.php',610,'feature_file_galleries,feature_file_galleries_rankings','tiki_p_view_file_gallery','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Upload  File','tiki-upload_file.php',615,'feature_file_galleries','tiki_p_view_file_gallery,tiki_p_upload_files','');
                                                                                                                                                                                                         
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'s','FAQs','tiki-list_faqs.php',650,'feature_faqs','tiki_p_view_faqs','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','List FAQs','tiki-list_faqs.php',665,'feature_faqs','tiki_p_view_faqs','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Admin FAQs','tiki-list_faqs.php',660,'feature_faqs','tiki_p_admin_faqs','');
                                                                                                                                                                                                         
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'s','Maps','tiki-map.phtml',700,'feature_maps','tiki_p_map_view','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Mapfiles','tiki-map_edit.php',705,'feature_maps','tiki_p_map_view','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Layer management','tiki-map_upload.php',710,'feature_maps','tiki_p_map_edit','');
                                                                                                                                                                                                         
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'s','Quizzes','tiki-list_quizzes.php',750,'feature_quizzes','','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','List quizzes','tiki-list_quizzes.php',755,'feature_quizzes','','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Quiz stats','tiki-quiz_stats.php',760,'feature_quizzes','tiki_p_view_quiz_stats','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Admin quiz','tiki-edit_quiz.php',765,'feature_quizzes','tiki_p_admin_quizzes','');
                                                                                                                                                                                                         
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'s','Trackers','tiki-list_trackers.php',800,'feature_trackers','','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','List trackers','tiki-list_trackers.php',805,'feature_trackers','','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Admin trackers','tiki-admin_trackers.php',810,'feature_trackers','tiki_p_admin_trackers','');
                                                                                                                                                                                                         
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'s','Surveys','tiki-list_surveys.php',850,'feature_surveys','','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','List surveys','tiki-list_surveys.php',855,'feature_surveys','','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Stats','tiki-surveys_stats.php',860,'feature_surveys','tiki_p_view_survey_stats','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Admin surveys','tiki-admin_surveys.php',865,'feature_surveys','tiki_p_admin_surveys','');
                                                                                                                                                                                                         
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'s','Newsletters','tiki-newsletters.php',900,'feature_newsletters','','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Send newsletters','tiki-send_newsletters.php',905,'feature_newsletters','tiki_p_admin_newsletters','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Admin newsletters','tiki-admin_newsletters.php',910,'feature_newsletters','tiki_p_admin_newsletters','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'s','Ephemerides','tiki-eph.php',950,'feature_eph','','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Admin','tiki-eph_admin.php',955,'feature_eph','tiki_p_eph_admin','');
                                                                                                                                                                                                         
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'s','Charts','tiki-charts.php',1000,'feature_charts','','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Charts','tiki-admin_charts.php',1005,'feature_charts','tiki_p_admin_charts','');
                                                                                                                                                                                                         
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'s','Admin (click!)','tiki-admin.php',1050,'','tiki_p_admin','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'s','Admin (click!)','tiki-admin.php',1050,'','tiki_p_admin_chat','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'s','Admin (click!)','tiki-admin.php',1050,'','tiki_p_admin_categories','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'s','Admin (click!)','tiki-admin.php',1050,'','tiki_p_admin_banners','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'s','Admin (click!)','tiki-admin.php',1050,'','tiki_p_edit_templates','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'s','Admin (click!)','tiki-admin.php',1050,'','tiki_p_edit_cookies','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'s','Admin (click!)','tiki-admin.php',1050,'','tiki_p_admin_dynamic','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'s','Admin (click!)','tiki-admin.php',1050,'','tiki_p_admin_mailin','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'s','Admin (click!)','tiki-admin.php',1050,'','tiki_p_edit_content_templates','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'s','Admin (click!)','tiki-admin.php',1050,'','tiki_p_edit_html_pages','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'s','Admin (click!)','tiki-admin.php',1050,'','tiki_p_view_referer_stats','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'s','Admin (click!)','tiki-admin.php',1050,'','tiki_p_admin_drawings','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'s','Admin (click!)','tiki-admin.php',1050,'','tiki_p_admin_shoutbox','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'s','Admin (click!)','tiki-admin.php',1050,'','tiki_p_admin_live_support','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'s','Admin (click!)','tiki-admin.php',1050,'','user_is_operator','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'s','Admin (click!)','tiki-admin.php',1050,'feature_integrator','tiki_p_admin_integrator','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Live support','tiki-live_support_admin.php',1055,'feature_live_support','tiki_p_live_support_admin','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Live support','tiki-live_support_admin.php',1055,'feature_live_support','user_is_operator','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Banning','tiki-admin_banning.php',1060,'feature_banning','tiki_p_admin_banning','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Calendar','tiki-admin_calendars.php',1065,'feature_calendar','tiki_p_admin_calendar','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Users','tiki-adminusers.php',1070,'','tiki_p_admin','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Groups','tiki-admingroups.php',1075,'','tiki_p_admin','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Cache','tiki-list_cache.php',1080,'','tiki_p_admin','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Modules','tiki-admin_modules.php',1085,'','tiki_p_admin','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Links','tiki-admin_links.php',1090,'','tiki_p_admin','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Hotwords','tiki-admin_hotwords.php',1095,'','tiki_p_admin','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','RSS modules','tiki-admin_rssmodules.php',1100,'','tiki_p_admin','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Menus','tiki-admin_menus.php',1105,'','tiki_p_admin','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Polls','tiki-admin_polls.php',1110,'','tiki_p_admin','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Backups','tiki-backup.php',1115,'','tiki_p_admin','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Mail notifications','tiki-admin_notifications.php',1120,'','tiki_p_admin','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Search stats','tiki-search_stats.php',1125,'','tiki_p_admin','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Theme control','tiki-theme_control.php',1130,'','tiki_p_admin','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','QuickTags','tiki-admin_quicktags.php',1135,'','tiki_p_admin','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Chat','tiki-admin_chat.php',1140,'','tiki_p_admin_chat','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Categories','tiki-admin_categories.php',1145,'','tiki_p_admin_categories','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Banners','tiki-list_banners.php',1150,'','tiki_p_admin_banners','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Edit templates','tiki-edit_templates.php',1155,'','tiki_p_edit_templates','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Drawings','tiki-admin_drawings.php',1160,'','tiki_p_admin_drawings','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Dynamic content','tiki-list_contents.php',1165,'','tiki_p_admin_dynamic','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Cookies','tiki-admin_cookies.php',1170,'','tiki_p_edit_cookies','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Mail-in','tiki-admin_mailin.php',1175,'','tiki_p_admin_mailin','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Content templates','tiki-admin_content_templates.php',1180,'','tiki_p_edit_content_templates','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','HTML pages','tiki-admin_html_pages.php',1185,'','tiki_p_edit_html_pages','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Shoutbox','tiki-shoutbox.php',1190,'','tiki_p_admin_shoutbox','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Referer stats','tiki-referer_stats.php',1195,'','tiki_p_view_referer_stats','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Edit languages','tiki-edit_languages.php',1200,'','tiki_p_edit_languages,lang_use_db','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Integrator','tiki-admin_integrator.php',1205,'feature_integrator','tiki_p_admin_integrator','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','Import PHPWiki Dump','tiki-import_phpwiki.php',1210,'','tiki_p_admin','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','phpinfo','tiki-phpinfo.php',1215,'','tiki_p_admin','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','DSN','tiki-admin_dsn.php',1220,'','tiki_p_admin','');
INSERT INTO tiki_menu_options (menuId,type,name,url,position,section,perm,groupname) VALUES (42,'o','External wikis','tiki-admin_external_wikis.php',1225,'','tiki_p_admin','');
# --------------------------------------------------------
                                                                                                                                                                                                         
#
# Table structure for table `tiki_menus`
#
# Creation: Jul 03, 2003 at 07:42 PM
# Last update: Jul 03, 2003 at 07:42 PM
#
                                                                                                                                                                                                         
DROP TABLE IF EXISTS tiki_menus;
CREATE TABLE tiki_menus (
  menuId int(8) NOT NULL auto_increment,
  name varchar(200) NOT NULL default '',
  description text,
  type char(1) default NULL,
  PRIMARY KEY  (menuId)
) TYPE=MyISAM AUTO_INCREMENT=1 ;
# --------------------------------------------------------
INSERT INTO tiki_menus (menuId,name,description,type) VALUES ('42','Application menu','Main extensive navigation menu','d');
# --------------------------------------------------------

