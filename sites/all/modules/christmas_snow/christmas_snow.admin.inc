<?php

/**
 * @file
 * Settings form.
 */

/**
 *
 * Form builder. Configure christmas_snow Storm.
 *
 */
function christmas_snow_admin_settings() {

  $form['christmas_snow_flakes_max'] = array(
    '#type' => 'select',
    '#title' => t('Maximum snow flakes'),
    '#default_value' => variable_get('christmas_snow_flakes_max', 128),
    '#options' => array(
      '16' => t('a flurry'),
      '32' => t('a snow fall'),
      '64' => t('heavy snow'),
      '128' => t('a blizzard'),
      '512' => t("nor'easter"),
    ),
    '#description' => t('Sets the maximum number of snowflakes that can exist on the screen at any given time.'),
  );

  $form['christmas_snow_color'] = array(
    '#type' => 'select',
    '#title' => t('Snow color'),
    '#default_value' => variable_get('christmas_snow_color', '#fff'),
    '#options' => array(
      "'#fff'" => t('white'),
      "'#99ccff'" => t('blue'),
      "'#ffff00'" => t('yellow'),
    ),
    '#description' => t("Don't eat (or use?) yellow snow."),
  );

  $form['christmas_snow_flake_bottom'] = array(
    '#type' => 'select',
    '#title' => t('Flakes on the bottom'),
    '#default_value' => variable_get('christmas_snow_flake_bottom', "null"),
    '#options' => array(
      "null" => 'auto',
      '500' => 'shallow',
      '750' => 'medium',
      '1000' => 'thick',
    ),
    '#description' => t("Limits the 'floor' (pixels) of the snow. If unspecified, snow will 'stick' to the bottom of the browser window and persists through browser resize/scrolling."),
  );

  $form['christmas_snow_follow_mouse'] = array(
    '#type' => 'select',
    '#title' => t('Flakes follow mouse'),
    '#default_value' => variable_get('christmas_snow_follow_mouse', "'true'"),
    '#options' => array(
      "'true'" => 'Yes',
      "'false'" => 'No',
    ),
    '#description' => t("Allows snow to move dynamically with the 'wind', relative to the mouse's X (left/right) coordinates."),
  );

  $form['christmas_snow_melt'] = array(
    '#type' => 'select',
    '#title' => t('Flakes melt away'),
    '#default_value' => variable_get('christmas_snow_melt', "'true'"),
    '#options' => array(
      "'true'" => 'Yes',
      "'false'" => 'No',
    ),
    '#description' => t("When recycling fallen snow (or rarely, when falling), have it 'melt' and fade out if browser supports it"),
  );

  $form['christmas_snow_stick'] = array(
    '#type' => 'select',
    '#title' => t('Flakes stick'),
    '#default_value' => variable_get('christmas_snow_stick', "'false'"),
    '#options' => array(
      "'false'" => 'Yes',
      "'true'" => 'No',
    ),
    '#description' => t("Allows the snow to 'stick' to the bottom of the window. When off, snow will never sit at the bottom."),
  );

  $form['christmas_snow_twinkle'] = array(
    '#type' => 'select',
    '#title' => t('Flakes twinkle'),
    '#default_value' => variable_get('christmas_snow_twinkle', "'false'"),
    '#options' => array(
      "'false'" => 'Yes',
      "'true'" => 'No',
    ),
    '#description' => t("Allow snow to randomly 'flicker' in and out of view while falling"),
  );

  $form['christmas_snow_character'] = array(
    '#type' => 'select',
    '#title' => t('The character for the flake'),
    '#default_value' => variable_get('christmas_snow_character', "'•'"),
    '#options' => array(
      "'•'" => "• (bullet)",
      "'·'" => "· (middot)",
    ),
    '#description' => t("&bull; (•) = bullet. &middot; entity (·) is not used as it's square on some systems etc. Changing this may result in cropping of the character and may require flakeWidth/flakeHeight changes, so be careful."),
  );

  $animation_options = array(
      '20' => t('fast & smooth - poor performance'),
      '33' => t('normal speed - moderate performance'),
      '50' => t('slow & choppy - conservative performance'),
    );

  $form['christmas_snow_animation_int'] = array(
    '#type' => 'select',
    '#title' => t('Performance'),
    '#default_value' => variable_get('christmas_snow_animation_int', 33),
    '#options' => $animation_options,
    '#description' => t("Theoretical 'miliseconds per frame' measurement. 20 = fast + smooth, but high CPU use. 50 = more conservative, but slower"),
  );

  return system_settings_form($form);
}

/**
 * Validate admin form submission
 */
function christmas_snow_admin_settings_validate($form, &$form_state) {
  $limit = $form_state['values']['christmas_snow_flakes_max'];
  if (!is_numeric($limit)) {
    form_set_error('christmas_snow_flakes_max', t('Please enter a number.'));
  }
}
