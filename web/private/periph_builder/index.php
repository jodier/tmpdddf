<?php include('php/periph_builder.php'); ?>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="fr" lang="fr">
  <head>
    <title>Arch Builder</title>

    <meta http-equiv="content-type" content="text/html; charset=UTF-8" />

    <link rel="icon" type="image/png" href="/favicon.png" />

    <style type="text/css" media="screen">
      @import "/bundles/hypnossupport/css/base.css";
      @import "/bundles/hypnossupport/css/menu.css";
      @import "/bundles/hypnossupport/css/menu2.css";
      @import "css/periph_builder.css";
    </style>

    <script type="text/javascript" src="/js/jquery.js"></script>
    <script type="text/javascript" src="/js/jquery_scroll.js"></script>

    <script type="text/javascript">

	/*-----------------------------------------------------------------*/
	/* INIT								   */
	/*-----------------------------------------------------------------*/

	$(document).ready(function() {

		var scrollingDiv = $('#editor_div');
 
		$(window).scroll(function() {

			scrollingDiv.stop().animate({'marginTop': $(window).scrollTop() + 'px'}, 'slow');
		});

		$('[name=' + 'remove' + ']').attr('disabled', true);
		$('[name=' + 'update' + ']').attr('disabled', true);
	});

	/*-----------------------------------------------------------------*/
	/* EDITLINE							   */
	/*-----------------------------------------------------------------*/

	function editLine(db, table, id)
	{
		$.getJSON('index.php?json=1&db=' + db + '&table=' + table + '&id=' + id, function(data) {

			__db = '';
			__login = '';
			__password = '';

			$.each(data, function(key, val) {

				$('[name=' + key + ']').val(val);

				/**/ if(key == 'name') {
					__db = val;
				}
				else if(key == 'login') {
					__login = val;
				}
				else if(key == 'password') {
					__password = val;
				}
			});

			__db = 'periph_builder_content_' + __db;

			$('.db').each(function() {
				this.value = __db;
			});
			$('.login').each(function() {
				this.value = __login;
			});
			$('.password').each(function() {
				this.value = __password;
			});

			$('[name=remove]').attr('disabled', false);
			$('[name=update]').attr('disabled', false);
		});

		$('.next').each(function() {

			idx = this.href.indexOf('&parent');

			if(idx >= 0)
			{
				this.href = this.href.substr(0, idx);
			}

			this.href = this.href + '&parent_table=' + table + '&parent_id=' + id;
		});
	}

	/*-----------------------------------------------------------------*/
	/* DISABLEBUTTONS						   */
	/*-----------------------------------------------------------------*/

	function disableButtons()
	{
		$('[name=id]').val('');

		$('[name=remove]').attr('disabled', true);
		$('[name=update]').attr('disabled', true);

		$(".next").each(function() {

			idx = this.href.indexOf('&parent');

			if(idx >= 0)
			{
				this.href = this.href.substr(0, idx);
			}
		});
	}

	/*-----------------------------------------------------------------*/

    </script>

  </head>
  <body>

    <div id="bar1_div">

      <div class="bar1_1_div">
        <a href="/"><img src="/bundles/hypnossupport/img/logo.png" width="424" height="60" alt="Hypnos3D" style="margin-left: 16px;" /></a>
      </div>

      <table class="bar1_2_div">
        <tr>
          <td><input type="text" name="A" /></td>
          <td style="width: 11px;"></td>
          <td><input type="image" src="/bundles/hypnossupport/img/search.png" name="B" /></td>
          <td style="width: 09px;"></td>
        </tr>
      </table>

    </div>

    <div id="bar2_div">

      <div class="bar2_1_div">

      <div class="bar2_1_div">

        <ul class="menu">
          <li>
            <a href="/">Home</a>
          </li>
          <li>
            <a href="/private/periph_builder">Arch Builder</a>
          </li>
        </ul>

      </div>

      </div>

      <div class="bar2_2_div">
        &nbsp;
        <a href=""><img src="/bundles/hypnossupport/img/FaceBook_gray.png" width="35" height="35" alt="FaceBook"
		onmouseout="javascript:this.src='/bundles/hypnossupport/img/FaceBook_gray.png'"
		onmouseover="javascript:this.src='/bundles/hypnossupport/img/FaceBook_color.png'" /></a>
        &nbsp;
        <a href=""><img src="/bundles/hypnossupport/img/Twitter_gray.png" width="35" height="35" alt="Twitter"
		onmouseout="javascript:this.src='/bundles/hypnossupport/img/Twitter_gray.png'"
		onmouseover="javascript:this.src='/bundles/hypnossupport/img/Twitter_color.png'" /></a>
        &nbsp;
        <a href=""><img src="/bundles/hypnossupport/img/Feed_gray.png" width="35" height="35" alt="Feed"
		onmouseout="javascript:this.src='/bundles/hypnossupport/img/Feed_gray.png'"
		onmouseover="javascript:this.src='/bundles/hypnossupport/img/Feed_color.png'" /></a>
        &nbsp;
        <a href=""><img src="/bundles/hypnossupport/img/Youtube_gray.png" width="35" height="35" alt="Youtube"
		onmouseout="javascript:this.src='/bundles/hypnossupport/img/Youtube_gray.png'"
		onmouseover="javascript:this.src='/bundles/hypnossupport/img/Youtube_color.png'" /></a>
        &nbsp;
      </div>

      <div style="float: left; margin-left: 10px; line-height: 35px;"><a href="/">home</a> &raquo; <a href="/private">private</a> &raquo; <a href="/private/periph_builder">periph_builder</a> /// <?php print("$table"); ?> ///</div>

    </div>

    <div id="bar3_div" style="background: #FFFFFF url('img/microcontroller.png') no-repeat left top;">

      <div id="bar3_1_div" style="width: 1150px; min-height: 550px;">
<?php if($db == 'periph_builder_config') { ?>
        <div id="list_div">
          <table class="periph_builder_list">
<?php $ConnectionBuilder->EmitHTMLContent(12); ?>
          </table>
        </div>

        <div id="editor_div">
          <form action="index.php" method="get" class="periph_builder_editor_form">
<?php $ConnectionBuilder->EmitHTMLForm(12, false); ?>
          </form>
          <form action="index.php" method="get" class="periph_builder_editor_form">
            <input type="hidden" name="db" class="db" />
            <input type="hidden" name="login" class="login" />
            <input type="hidden" name="password" class="password" />
            <input type="submit" value="Goto" />
          </form>
        </div>
<?php } else { ?>
        <div id="list_div">
          <table class="periph_builder_list">
<?php $ArchBuilder->EmitHTMLContent(12); ?>
          </table>
        </div>

        <div id="editor_div">
          <form action="index.php" method="get" class="periph_builder_editor_form">
<?php $ArchBuilder->EmitHTMLForm(12, true); ?>
          </form>
        </div>
<?php } ?>
      </div>

    </div>

    <div id="bar4_div">
      <div style="padding: 6px; font-size: 12px; text-align: center;">&copy; Copyright 2012 Hypnos3D. All Rights Reserved.</div>
    </div>

  </body>
</html>
