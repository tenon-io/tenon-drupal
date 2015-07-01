/**
 * jQuery behaviors for platform notification feeds.
 */
(function ($) {
  Drupal.behaviors.tenon_notifications = {
    attach: function (context, settings) {
      // Setup.
      var items = [];
      var menuLinkSel = 'a[data-tenon="page_report"]';
      if ($(menuLinkSel + '.tenon-notifications-processed').length) {
        return;
      }
      $(menuLinkSel).attr('href', '#').addClass('tenon-notifications-processed');
      // Only continues if we have the hopscotch library defined.
      if (typeof hopscotch == 'undefined') {
        return;
      }

      // Prepare data for the API request.
      var i = 0;
      var data = '';
      var api_parameters = {
        'key': settings.tenon.key,
        'url': settings.tenon.url,
        'appId': settings.tenon.appId,
        'projectID': settings.tenon.projectID,
        'certainty': settings.tenon.certainty,
        'importance': settings.tenon.importance,
        'level': settings.tenon.level,
        'priority': settings.tenon.priority,
        'ref': settings.tenon.ref,
        'store': settings.tenon.store,
        'uaString': settings.tenon.uaString,
        'viewPortHeight': settings.tenon.viewPortHeight,
        'viewPortWidth': settings.tenon.viewPortWidth
      };

      for (var parameter in api_parameters ) {
        var separator = '';
        if (i++ != 0) {
          separator = '&';
        }
        data += separator + parameter + '=' + encodeURIComponent(api_parameters[parameter]);
      }

      // Do the actual call.
      var url = 'http://tenon.io/api/';
      var options = {
        'headers': {
          'Content-Type': 'application/x-www-form-urlencoded'
        },
        'method': 'POST',
        'data': data
      };
      //$.post(url, options);

      // Format the report content.
      var report_id = 13456;
      var entry = {
        content: '<p>3 issues</p>',
        link: 'https://tenon.io/report.php?id=' + report_id,
        title: 'Your page accessibility report'
      };
      var item = tenon_notification_build_item(entry);
      items.push(item);

      // If there are items to display in a hopscotch tour...
      if (items.length) {
        // Sets up the DOM elements.
        // @TODO: Plug the persistent reports.
        $(menuLinkSel).append($("<span id='tenon-report-summary-count'>1</span>"));
        
        // Sets up the tour object with the loaded feed item steps.
        var tour = {
          showPrevButton: true,
          scrollTopMargin: 100,
          id: "tenon-notifications",
          steps: items,
          onEnd: function () {
            //os_tour_notifications_count(-1);
            //os_tour_notifications_read_update();
          }
        };

        // Adds our tour overlay behavior with desired effects.
        $('a[data-tenon="page_report"]').click(function (e) {
          // Close the drawer when we generate the report.
          Drupal.cp_toolbar.drawer_close();

          $('html, body').animate({scrollTop: 0}, '500', 'swing', function () {
            $('.hopscotch-bubble').addClass('animated');
            hopscotch.startTour(tour);

            // Removes animation for each step and let us to target just this tour in CSS rules.
            $('.hopscotch-bubble').removeClass('animated').addClass('tenon-notifications');
          });
        });
      }
    }
  };

  /**
   * Format Tour content.
   *
   * @param entry
   *   Object with the following keys:
   *   - title: Title of the tour step.
   *   - content: Content to display for this step.
   *   - link: Link to read more details about the step.
   *
   * @returns {{title: string, content: string, target: Element, placement: string, yOffset: number, xOffset: number}}
   */
  function tenon_notification_build_item(entry) {
    // Prepare the output to display inside the tour's content region.
    var output = "<div class='feed_item'>";

    // Builds the remainder of the content, with a "Read more" link.
    output += "<span class='description'>" + entry.content + "</span>";
    output += '<div class="tenon-notifications-readmore"><a target="_blank" href="' + entry.link + '">Read more &raquo;</a></div></div>';

    // Returns the item to be added to the tour's (array) `items` property .
    var item = {
      title: entry.title,
      content: output,
      target: document.querySelector('.tenon-link'),
      placement: "bottom",
      yOffset: -3,
      xOffset: -10,
    };
    return item;
  }
})(jQuery);
