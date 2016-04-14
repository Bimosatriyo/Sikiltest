<?php
/**
 * Implements hook_html_head_alter().
 * This will overwrite the default meta character type tag with HTML5 version.
 */
function dark_elegant_html_head_alter(&$head_elements) {
  $head_elements['system_meta_content_type']['#attributes'] = array(
    'charset' => 'utf-8'
  );
}

/**
 * Insert themed breadcrumb page navigation at top of the node content.
 */
function dark_elegant_breadcrumb($variables) {
  $breadcrumb = $variables['breadcrumb'];
  if (!empty($breadcrumb)) {
    // Use CSS to hide titile .element-invisible.
    $output = '<h2 class="element-invisible">' . t('You are here') . '</h2>';
    // comment below line to hide current page to breadcrumb
$breadcrumb[] = drupal_get_title();
    $output .= '<nav class="breadcrumb">' . implode(' » ', $breadcrumb) . '</nav>';
    return $output;
  }
}

function dark_elegant_preprocess_html(&$vars) {
  // Add body classes for custom design options
  $sidebar_layout = theme_get_setting('sidebar_layout', 'dark_elegant');
  if($sidebar_layout == 'left_sidebar') {
    $vars['classes_array'][] = 'left-sidebar';
  }
}

/**
 * Override or insert variables into the page template.
 */
function dark_elegant_preprocess_page(&$vars) {
  if (isset($vars['main_menu'])) {
    $vars['main_menu'] = theme('links__system_main_menu', array(
      'links' => $vars['main_menu'],
      'attributes' => array(
        'class' => array('links', 'main-menu', 'clearfix'),
      ),
      'heading' => array(
        'text' => t('Main menu'),
        'level' => 'h2',
        'class' => array('element-invisible'),
      )
    ));
  }
  else {
    $vars['main_menu'] = FALSE;
  }
  if (isset($vars['secondary_menu'])) {
    $vars['secondary_menu'] = theme('links__system_secondary_menu', array(
      'links' => $vars['secondary_menu'],
      'attributes' => array(
        'class' => array('links', 'secondary-menu', 'clearfix'),
      ),
      'heading' => array(
        'text' => t('Secondary menu'),
        'level' => 'h2',
        'class' => array('element-invisible'),
      )
    ));
  }
  else {
    $vars['secondary_menu'] = FALSE;
  }
}

/**
 * Duplicate of theme_menu_local_tasks() but adds clearfix to tabs.
 */
function dark_elegant_menu_local_tasks(&$variables) {
  $output = '';

  if (!empty($variables['primary'])) {
    $variables['primary']['#prefix'] = '<h2 class="element-invisible">' . t('Primary tabs') . '</h2>';
    $variables['primary']['#prefix'] .= '<ul class="tabs primary clearfix">';
    $variables['primary']['#suffix'] = '</ul>';
    $output .= drupal_render($variables['primary']);
  }
  if (!empty($variables['secondary'])) {
    $variables['secondary']['#prefix'] = '<h2 class="element-invisible">' . t('Secondary tabs') . '</h2>';
    $variables['secondary']['#prefix'] .= '<ul class="tabs secondary clearfix">';
    $variables['secondary']['#suffix'] = '</ul>';
    $output .= drupal_render($variables['secondary']);
  }
  return $output;
}

/**
 * Override or insert variables into the node template.
 */
function dark_elegant_preprocess_node(&$variables) {
  $node = $variables['node'];
  if ($variables['view_mode'] == 'full' && node_is_page($variables['node'])) {
    $variables['classes_array'][] = 'node-full';
  }
  $variables['date'] = t('!datetime', array('!datetime' =>  date('l, j F Y', $variables['created'])));
}

/**
 * Login register dll.
 */
function dark_elegant_theme() {
  $items = array();
    
  $items['user_login'] = array(
    'render element' => 'form',
    'path' => drupal_get_path('theme', 'dark_elegant') . '/templates',
    'template' => 'user-login',
    'preprocess functions' => array(
       'dark_elegant_preprocess_user_login'
    ),
  );
  $items['user_register_form'] = array(
    'render element' => 'form',
    'path' => drupal_get_path('theme', 'dark_elegant') . '/templates',
    'template' => 'user-register-form',
    'preprocess functions' => array(
      'dark_elegant_preprocess_user_register_form'
    ),
  );
  $items['user_pass'] = array(
    'render element' => 'form',
    'path' => drupal_get_path('theme', 'dark_elegant') . '/templates',
    'template' => 'user-pass',
    'preprocess functions' => array(
      'dark_elegant_preprocess_user_pass'
    ),
  );
  return $items;
}

/**
 * implement preprocess
 */
 function dark_elegant_preprocess_user_login(&$vars) {
  $vars['intro_text'] = t('Login alo udah terdaftar secara sah');
}

function dark_elegant_preprocess_user_register_form(&$vars) {
  $vars['intro_text'] = t('Register aja kalo belom punya akun disini');
}

function dark_elegant_preprocess_user_pass(&$vars) {
  $vars['intro_text'] = t('Kalo passwordnya khilap pencet ini aja');
}
/**
 * Add javascript files for front-page jquery slideshow.
 */
if (drupal_is_front_page()) {
  drupal_add_js(drupal_get_path('theme', 'dark_elegant') . '/js/jquery.flexslider-min.js');
  drupal_add_js(drupal_get_path('theme', 'dark_elegant') . '/js/slide.js');
}
