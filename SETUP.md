**LINE for 3DS setup instructions**

# Index
- [Index](#index)
- [Overview](#overview)
- [What you need](#what-you-need)
- [Create your LINE account](#create-your-line-account)
- [Create your LINE BOT (3DS account)](#create-your-line-bot-3ds-account)
  - [Create your LINE developer account](#create-your-line-developer-account)
  - [Create your LINE BOT](#create-your-line-bot)
- [Set up your Google drive](#set-up-your-google-drive)
  - [Create folder and files](#create-folder-and-files)
  - [Set up your Google Apps Script](#set-up-your-google-apps-script)
  - [Finalize your Google Apps Script setup](#finalize-your-google-apps-script-setup)
- [Set up your 3DS](#set-up-your-3ds)
- [FAQ](#faq)
  - [How do I (or let someone) add my 3DS account as a friend?????](#how-do-i-or-let-someone-add-my-3ds-account-as-a-friend)
  - [How do I set up Google Apps Script URL on my 3DS if camera is not working?????](#how-do-i-set-up-google-apps-script-url-on-my-3ds-if-camera-is-not-working)
  - [How do I check my Google Apps Script URL?????](#how-do-i-check-my-google-apps-script-url)
  - [Why do I get a auto reply message when sending message to my 3DS (or adding 3DS account as a friend)?????](#why-do-i-get-a-auto-reply-message-when-sending-message-to-my-3ds-or-adding-3ds-account-as-a-friend)
  - [Why do I get an error message while setting Google Apps Script on my 3DS?????](#why-do-i-get-an-error-message-while-setting-google-apps-script-on-my-3ds)
  - [Why Create a Messaging API channel is disabled?????](#why-create-a-messaging-api-channel-is-disabled)
  - [Why Google Apps Script says "authorization required" or "review permission"?????](#why-google-apps-script-says-authorization-required-or-review-permission)

# Overview

LINE for 3DS uses:
* `LINE BOT API` (green one) to communicate with normal LINE user.
* `Google drive` (red one) for storage (images, audio, etc...).
  * `Google Spreadsheets` for storage (chat logs).
  * `Google Apps Script` to send/receive messages to/from LINE server.
* `3DS` (pink one) to run LINE for 3DS application.
   ![Line_for_3DS_overview](https://raw.githubusercontent.com/Core-2-Extreme/Line_for_3DS/master/setup_resources/Line_for_3DS_overview.png)

As you can see, 3DS ***never*** talks with `LINE server` directly, instead 3DS will talk with `Google Apps Script` (i.e. Google server).

# What you need

To set up LINE for 3DS, you need
* `PC` and `phone` (at least one of them, having both of them are ***strongly*** recommended).
* `Unrestricted Internet connection` to `Google` and `LINE` (and `Apple` if you are using iOS) servers.
* `Google account` (and `Apple account` (iOS)) (and only in some country, `SMS capable phone number`).
* **MOST IMPORTANT ONE** : `Patience` (at least enough to complete the setup steps without skipping any steps).

# Create your LINE account

If you already have LINE account, move on to the [next step](#create-your-line-bot-3ds-account).

![Line_for_3DS_overview_with_circle_and_arrows_0](https://raw.githubusercontent.com/Core-2-Extreme/Line_for_3DS/master/setup_resources/Line_for_3DS_overview_with_circle_and_arrows_0.png)

If you haven't installed LINE app (<a href="https://apps.apple.com/us/app/line/id443904275" target="_blank">iOS</a>, <a href="https://play.google.com/store/apps/details?id=jp.naver.line.android&hl=en&gl=US" target="_blank">Android</a>) on your phone (or Android emullator), install it first.
1. Open LINE app and tap on `Sign up`.
2. Then select country.
   * ***As of January 2024***, if you select **Japan**, **Hong Kong**, **Taiwan**, **Korea** or **Thailand** you are required to verify phone number otherwise you need to log in with your Google account (Android) or Apple account (iOS). 
3. Then enter a phone number and continue or log in with Google or Apple.
4. Set up your account (account name can be anything).
5. Congratrations!!!!!, you've successfully made your account!!!!!
   ![Line_setup_0](https://raw.githubusercontent.com/Core-2-Extreme/Line_for_3DS/master/setup_resources/Line_setup_0.png)


# Create your LINE BOT (3DS account)

Now, you have LINE account on your phone (or Android emulator), then you need to make a LINE BOT that will be your 3DS account.

![Line_for_3DS_overview_with_circle_and_arrows_1](https://raw.githubusercontent.com/Core-2-Extreme/Line_for_3DS/master/setup_resources/Line_for_3DS_overview_with_circle_and_arrows_1.png)

## Create your LINE developer account

If you already have LINE developer account, move on to the [next step](#create-your-line-bot).

First, we need to make a LINE developer account.
1. Go to <a href="https://developers.line.biz/en/" target="_blank">LINE developer</a> and click on `Log in to Console`.
2. Then select `Log in with LINE account` to log in.
3. If this is the first time to log in to LINE developer, you'll be prompted to make a business account, if so click on `Create LINE Business ID` to make it.
4. Set up your account (developer name can be anything).
   ![Line_developer_setup_0](https://raw.githubusercontent.com/Core-2-Extreme/Line_for_3DS/master/setup_resources/Line_developer_setup_0.png)
5. Congratrations!!!!!, you've successfully made your LINE developer account!!!!!
   ![Line_developer_setup_1](https://raw.githubusercontent.com/Core-2-Extreme/Line_for_3DS/master/setup_resources/Line_developer_setup_1.png)

## Create your LINE BOT

If you already have a LINE BOT, move on to the [next step](#set-up-your-google-drive).

Then, let's create a LINE BOT.
1. Click on `Create a new provider`
2. Enter provider name (any name) and click on `Create`.
3. Next, you need to create a BOT channel, click on `Create a Messaging API channel` (If you can't create it, see [Why Create a Messaging API channel is disabled?????](#why-create-a-messaging-api-channel-is-disabled)).
   ![Line_bot_setup_0](https://raw.githubusercontent.com/Core-2-Extreme/Line_for_3DS/master/setup_resources/Line_bot_setup_0.png)
4. Fill the information and continue.
   * `Country or region` : Can be anywhere.
   * `Channel icon` : Can be anything, this will be your account icon (see green square).
   * `Channel name` : Can be anything, this will be your account name (see red square).
   * `Channel description` : Can be anything.
   * `Category` : Can be anything.
   * `Subcategory` : Can be anything.
   ![Line_bot_setup_1](https://raw.githubusercontent.com/Core-2-Extreme/Line_for_3DS/master/setup_resources/Line_bot_setup_1.png)
5. Click on `Messaging API`
6. Go to bottom of the page and click on `Issue` to generate your access token. Also you can change account settings if you want (you can do it anytime).
   * `Allow bot to join group chat` : Whether your 3DS account can join group (not DM) chat (**recommendation : enable**).
   * `Auto-reply messages` : Whether your 3DS account automatically send a reply message when it receives messages (**recommendation : disable**).
   * `Greeting messages` : Whether your 3DS account automatically send a greeting message when someone follow (add as a friend) your 3DS account (**recommendation : disable**).
7. Congratrations!!!!!, you've successfully made your BOT account!!!!! We'll use this page later so don't close this tab.
   ![Line_bot_setup_2](https://raw.githubusercontent.com/Core-2-Extreme/Line_for_3DS/master/setup_resources/Line_bot_setup_2.png)


# Set up your Google drive

In this step, we'll set up `Google Sheets` (acts as a chat logs storage) and `Google Apps Script` (acts as a server).

![Line_for_3DS_overview_with_circle_and_arrows_2](https://raw.githubusercontent.com/Core-2-Extreme/Line_for_3DS/master/setup_resources/Line_for_3DS_overview_with_circle_and_arrows_2.png)

## Create folder and files

First, let's create folder and files on your Google drive.
1. Go to <a href="https://drive.google.com/drive/my-drive/" target="_blank">Google drive</a> and log in to your Google drive (create a Google account if you don't have it).
2. After logging in, click on `New`.
3. Click on `New folder` to make a new folder then name it `LINE_for_3DS`.
4. Open `LINE_for_3DS` folder
5. Create `Google Sheets` and `Google Apps Script` and don't close opend tabs!!!!!
6. If you get a warning when creating Google Apps Script, click on `Create script`.
7. Optionally, you can rename (any name) your `Google Sheets` and `Google Apps Script`.
8. Congratrations!!!!!, you've successfully prepared your Google drive!!!!!
   ![Google_drive_setup_0](https://raw.githubusercontent.com/Core-2-Extreme/Line_for_3DS/master/setup_resources/Google_drive_setup_0.png)

## Set up your Google Apps Script

Then, let's set up your Google Apps Script.
1. Open your `Google Apps Script` and **DELETE** ***ALL OF*** code to empty your script first.
2. After emptying your script, **copy & paste** ***ALL OF*** <a href="https://raw.githubusercontent.com/Core-2-Extreme/Line_for_3DS/master/https://raw.githubusercontent.com/Core-2-Extreme/Line_for_3DS/master/setup_resources/gas/script.txt" target="_blank">this script</a> on your `Google Apps Script` and save it (click on `save icon` or press `CTRL + S`).
3. Next, check if script works fine by selecting `Test_spot` and click on `Run`.
4. If you are prompted to review permission, grant access to your script (see [Why Google Apps Script says "authorization required" or "review permission"?????](#why-google-apps-script-says-authorization-required-or-review-permission) if you don't know how to do that).
   ![Google_apps_script_setup_0](https://raw.githubusercontent.com/Core-2-Extreme/Line_for_3DS/master/setup_resources/Google_apps_script_setup_0.png)
5. Then, you shoud see version number and `OK` in the log box (if you don't see log box, click on `Execution log`), if you see something else such as `ReferenceError: Copy_paste_checker is not defined` it means you haven't copy & pasted **ALL OF** script code, in that case try again from step 1.
   ![Google_apps_script_setup_1](https://raw.githubusercontent.com/Core-2-Extreme/Line_for_3DS/master/setup_resources/Google_apps_script_setup_1.png)
6. Set account information to your Google Apps Script.
   * `APP_USER_ACCESS_TOKEN` : Access token for your LINE BOT account. Go back to LINE developer tab, copy channel access token (click on `copy icon`) and paste it.
   * `APP_USER_GOOGLE_SHEET_ID` : Google sheets ID for chat logs. Go back to Google sheets tab, copy sheet ID without `/` (slash).
   * `APP_USER_CLIENT_NAME` : Your BOT account name, this is only shown on your 3DS and can be different from your LINE BOT account name.
   * `APP_USER_TIMEZONE` : Timezone for chat logs, this is only shown on your 3DS and can be different from your actual timezone.
   * `APP_USER_TIME_FORMAT` : Time format for chat logs, this is only shown on your 3DS.
   * `APP_USER_PASSWORD` : Your password (make something new, can be anything except special (non-1byte) character).
7. Click on `Deploy` and `New deployment`.
8. Click on `Settings icon` and `Web app`.
9. Set parameters as below then click on `Deploy`.
   * `Execute as` : **Me** so that script can access your Google Drive.
   * `Who has access` : **Anyone** so that your 3DS and LINE server can access.
10. If you are prompted to review permission, grant access to your script (see [Why Google Apps Script says "authorization required" or "review permission"?????](#why-google-apps-script-says-authorization-required-or-review-permission) if you don't know how to do that).
11. Click on `Copy` to copy your script URL (we'll use it in next step), finally click on `Done`.
12. Congratrations!!!!!, you've (almost) finished setting up your Google Apps Script!!!!!
   ![Google_apps_script_setup_2](https://raw.githubusercontent.com/Core-2-Extreme/Line_for_3DS/master/setup_resources/Google_apps_script_setup_2.png)

## Finalize your Google Apps Script setup

After deploying your script, check if everything goes fine until now first, then perform the final step of the Google Apps Script setup.
1. Paste your script URL (see [How do I check my Google Apps Script URL?????](#how-do-i-check-my-google-apps-script-url) if you don't know how to check your Google Apps Script URL) to your browser, add `?op=setup` to it (so your URL should be `https://script.google.com/macros/s/xxxxxxxxxxxxxxxxxxxxxxx/exec?op=setup` now) and open it. ***It may take a while to load the page, so be patient, don't close the tab!!!!!***.
2. If everything goes fine until now, you should ***only see*** an error for `Webhook settings validation` (we need to manually enable use of webhook because **LINE DOES NOT PROVIDE SUCH API TO US!!!!!**).
3. Go back to LINE developer tab, and click on `Use webhook` to enable it.
4. Go back to your script tab and reload it. You should see no errors (and you should see QR code instead).
5. Congratrations!!!!!, you've successfully configure your Google Apps Script!!!!! We'll use this page later so don't close this tab.
   ![Google_apps_script_finalizing_0](https://raw.githubusercontent.com/Core-2-Extreme/Line_for_3DS/master/setup_resources/Google_apps_script_finalizing_0.png)


# Set up your 3DS

We are almost there, this is the final step of the setup.

![Line_for_3DS_overview_with_circle_and_arrows_3](https://raw.githubusercontent.com/Core-2-Extreme/Line_for_3DS/master/setup_resources/Line_for_3DS_overview_with_circle_and_arrows_3.png)

1. Install and open LINE for 3DS on your 3DS.
2. Touch on LINE icon to open it.
3. Type your Google Apps Script password (`APP_USER_PASSWORD`). Unlike older version (v1.x.x), you **MUST** type your Google Apps Script password or it will NOT work at all.
4. Touch on Change GAS URL (or press `X` button) and select camera (if your camera is not working, see [How do I set up Google Apps Script URL on my 3DS if camera is not working?????](#how-do-i-set-up-google-apps-script-url-on-my-3ds-if-camera-is-not-working)).
5. Go back to [Google Apps Script setup page (step 5)](#finalize-your-google-apps-script-setup) and scan the QR code.
6. Be patient until `Please wait` message disappear (if you get any error messages, see [Why do I get an error message while setting Google Apps Script on my 3DS?????](#why-do-i-get-an-error-message-while-setting-google-apps-script-on-my-3ds)).
   ![Line_for_3ds_setup_0](https://raw.githubusercontent.com/Core-2-Extreme/Line_for_3DS/master/setup_resources/Line_for_3ds_setup_0.png)
7. Add your BOT account (3DS account) as a friend on your LINE account (if you don't know how to do that, see [How do I (or let someone) add my 3DS account as a friend?????](#how-do-i-or-let-someone-add-my-3ds-account-as-a-friend)).
8. Touch on `IDs` room.
9. Touch on `Receive` then touch on `Update logs` (or press `B` button).
10. You should see `Touch to add this ID`, touch on it.
11. Touch on `Confirm` (right button) without editing text.
12. New chat room should be added, touch on it.
13. Touch on `Receive` then touch on `Update logs` (or press `B` button) (same as step 9).
14. You should see followed message (and chat logs if you send something to your 3DS account).
15. Touch on `Send` then touch on `Send a message` (or press `A` button) to send something from your 3DS.
16. You should see message on both your 3DS and official LINE application.
17. Congratrations!!!!!, you've successfully set up LINE for 3DS!!!!!
   ![Line_for_3ds_setup_1](https://raw.githubusercontent.com/Core-2-Extreme/Line_for_3DS/master/setup_resources/Line_for_3ds_setup_1.png)


# FAQ

## How do I (or let someone) add my 3DS account as a friend?????

If you haven't set up your LINE developer account and LINE BOT, set up them first (see [Create your LINE BOT (3DS account)](#create-your-line-bot-3ds-account) if you don't know how to do it).

To add your 3DS account as a friend, you can use either QR code or ID.
1. Go to <a href="https://developers.line.biz/en/" target="_blank">LINE developer</a> and log in to your developer account.
2. Click on `Messaging API`.
   * `Scan QR code` on your device (or let someone scan it)
   * Add friend via `BOT basic ID` (include `@` mark, e.g. **@fdbnoi4**) (or let someone add it).
   ![Line_bot_qr_and_id_0](https://raw.githubusercontent.com/Core-2-Extreme/Line_for_3DS/master/setup_resources/Line_bot_qr_and_id_0.png)

## How do I set up Google Apps Script URL on my 3DS if camera is not working?????

If you can't scan the QR code on your 3DS, you can use URL shortener such as [t.ly](https://t.ly/) and [reduced.to](https://reduced.to/).
1. Shorten your Google Apps Script with URL shortener (see [How do I check my Google Apps Script URL?????](#how-do-i-check-my-google-apps-script-url) if you don't know how to check your Google Apps Script URL).
2. Touch on `Change GAS URL` and `keyboard`.
3. Type your short URL and touch on `confirm`.
4. Be patient until `Please wait` message disappear (if you get any error messages, see [Why do I get an error message while setting Google Apps Script on my 3DS?????](#why-do-i-get-an-error-message-while-setting-google-apps-script-on-my-3ds)).
   ![Line_for_3ds_setup_short_url_0](https://raw.githubusercontent.com/Core-2-Extreme/Line_for_3DS/master/setup_resources/Line_for_3ds_setup_short_url_0.png)

## How do I check my Google Apps Script URL?????

If you haven't deployed your Google Apps Script, you need to **deploy** it first (see [Set up your Google Apps Script (step 7-11)](#set-up-your-google-apps-script) if you don't know how to do it).

If you forget your Google Apps Script URL after deploying it, you can check it with following steps.
1. Open your Google Apps Script.
2. Click on `Deploy` and `Manage deployments`.
3. You should see your Google Apps Script URL under Web app section.
   ![Google_apps_script_url_0](https://raw.githubusercontent.com/Core-2-Extreme/Line_for_3DS/master/setup_resources/Google_apps_script_url_0.png)

## Why do I get a auto reply message when sending message to my 3DS (or adding 3DS account as a friend)?????

This is because LINE BOT API has features called `Auto-reply messages` and `Greeting messages` and they are enabled by default.

You can disable them with following steps.
1. Go to <a href="https://developers.line.biz/en/" target="_blank">LINE developer</a> and log in to your developer account.
2. Click on `Messaging API` and scroll down.
3. Disable `Auto-reply messages` and/or `Greeting messages` as you like.
   ![Line_bot_auto_reply_0](https://raw.githubusercontent.com/Core-2-Extreme/Line_for_3DS/master/setup_resources/Line_bot_auto_reply_0.png)

## Why do I get an error message while setting Google Apps Script on my 3DS?????

If you get an error something like `curl returned NOT success` or `curl_easy_perform() failed` it most likely either:
* Your Internet connection is unstable (or disconnected).
* Using wrong short URL (typo) (If you are using short URL method).
  * If you are sure that URL is not wrong, try using different URL shortener.
* Using wrong Google Apps script URL when creating short URL (if you are using short URL method).
  * If you are sure that you've specified correct Google Apps Script URL, try making different Google Apps Script and try again. 

If you get an error something like `jsmn returned NOT success` it most likely either:
* You haven't deployed or deployed your Google Apps Script with wrong permission (see [Set up your Google Apps Script (step 7-11)](#set-up-your-google-apps-script) for detail).
  * If you are sure that you've deployed your Google Apps Script with correct permissions, try making different Google Apps Script and try again. 
* Using wrong short URL (typo) (If you are using short URL method).
  * If you are sure that URL is not wrong, try using different URL shortener.
* Using wrong Google Apps script URL when creating short URL (if you are using short URL method).
  * If you are sure that you've specified correct Google Apps Script URL, try making different Google Apps Script and try again. 

## Why Create a Messaging API channel is disabled?????

Some people report this problem while exact reason is still unknown, it seems `Messaging API` is not available in some country.

Possible workarounds:
   * Use different country when [creating your LINE account](#create-your-line-account).

## Why Google Apps Script says "authorization required" or "review permission"?????

Some people report that *if you log in with multiple account*, **this step fails**.

Possible workarounds:
   * Use private browsing mode and log in with only 1 account then try again.
   * Use different browser and log in with only 1 account then try again.
   * Log out and remove all accounts and log in with only 1 account then try again.

If you get this message, you need to grant permissions to your Google Apps Script.
1. Click on `Review permissions`,
2. Select your Google account (select Google account that you used to create this script, otherwise it won't work as you expected)
3. Click on `Advanced` (screenshot says `Hide advanced` because I already clicked it) and click on `Go to {Your Google Apps Script name} (unsafe)`
4. Finally, click on `Allow` to grant permission to your script.
   ![Google_apps_script_review_permissions_0](https://raw.githubusercontent.com/Core-2-Extreme/Line_for_3DS/master/setup_resources/Google_apps_script_review_permissions_0.png)
