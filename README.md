#Tenon Drupal Module

Note: This module supports both Drupal and Open Scholar

## Get a Tenon.io account before continuing
If you do not already have an account on Tenon.io, you need one to use this. Head over to [https://tenon.io/register.php](https://tenon.io/register.php). 

Once you're registered and confirmed, get your API key at [http://tenon.io/apikey.php](http://tenon.io/apikey.php)

## Installation
Install this just as you would any other Drupal Module: [https://www.drupal.org/documentation/install/modules-themes/modules-7](https://www.drupal.org/documentation/install/modules-themes/modules-7)

## Configuration
Navigate to your site's Settings area where you'll find a link to "Tenon.io settings". Enter your API key in the settings screen.

There are also some advanced API settings available. You can learn more about what they are and how they work by viewing [Tenon's documentation](http://tenon.io/documentation/understanding-request-parameters.php)	

## Use it
This module adds a new menu option named "Accessibility Check". When you click it, the Module will send the page you're currently viewing to Tenon's API to check it for accessibility.  

When the page has been checked, you'll see a summary of how many issues are on the page and a link that says "View full report".  Activate that link to see the detailed report for the page. 
