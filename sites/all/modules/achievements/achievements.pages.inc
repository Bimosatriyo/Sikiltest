<?php

/**
 * @file
 * Page callbacks for the Achievements module.
 */

/**
 * Menu callback; show the site-wide leaderboard.
 *
 * @see achievement_leaderboard_block()
 */
function achievements_leaderboard_totals() {
  $header = array(array('data' => t('#'), 'sort' => NULL), t('Who'), t('Points'), t('Unlocks'), t('Latest achievement'));
  $achievers = $rows = array('top' => array(), 'relative' => array());

  $query = db_select('achievement_totals', 'at')->extend('PagerDefault');
  $query->addTag('achievement_totals')->join('users', 'u', 'u.uid = at.uid');
  $query->fields('at', array('uid', 'points', 'unlocks', 'timestamp', 'achievement_id'))->fields('u', array('name'));
  $query->orderBy('at.points', 'DESC')->orderBy('at.timestamp'); 
  $query->limit(variable_get('achievements_leaderboard_count_per_page', 10));
  $achievers['top'] = $query->execute()->fetchAllAssoc('uid');

  if (user_is_logged_in() && variable_get('achievements_leaderboard_relative', 'nearby_ranks') != 'disabled') {
    $achievers['relative'] = (variable_get('achievements_leaderboard_relative', 'nearby_ranks') == 'nearby_ranks')
      ? achievements_totals_user(variable_get('achievements_leaderboard_relative_nearby_ranks', 2))
      : achievements_totals_user(); 
  }

  $rank = 0 + ($GLOBALS['pager_page_array'][0] * variable_get('achievements_leaderboard_count_per_page', 10));
  foreach (array('top', 'relative') as $type) {
    foreach ($achievers[$type] as $achiever) {
      $rank = $achiever->rank = isset($achiever->rank) ? $achiever->rank : $rank + 1;
      $classes = ($achiever->uid == $GLOBALS['user']->uid) ? array('achievement-leaderboard-current-user') : array();
      if ($rank <= 3) {  
        $classes[] = 'achievement-leaderboard-top-rank';
        $classes[] = 'achievement-leaderboard-top-rank-' . $rank;
      }

      $rows[$type][$achiever->uid] = array(
        'class' => $classes,
        'data'  => array(
          array(
            'data'  => $rank,
            'class' => array('achievement-leaderboard-rank')
          ),
          array(
            'data'  => theme('username', array('account' => $achiever)),
            'class' => array('achievement-leaderboard-username')
          ),
          array(
            'data'  => l($achiever->points, 'user/' . $achiever->uid . '/achievements'),
            'class' => array('achievement-leaderboard-points')
          ),
          array(
            'data'  => l($achiever->unlocks, 'user/' . $achiever->uid . '/achievements'),
            'class' => array('achievement-leaderboard-unlocks')
          ),
        ),
      );

     $achievers[$type][$achiever->uid]->latest = array(
        '#theme'        => 'achievement_latest_unlock',
        '#achievement'  => achievements_load($achiever->achievement_id),
        '#unlock'       => array('timestamp' => $achiever->timestamp),
      );
      $rows[$type][$achiever->uid]['data'][] = array(
        'data'    => render($achievers[$type][$achiever->uid]->latest),
        'class'   => array('achievement-leaderboard-latest')
      );
    }
  }

  foreach (array('top', 'relative') as $type) {
    $leaderboard = array(
      'achievers' => $achievers,
      'block'     => FALSE,
      'type'      => $type,
      'render'    => array(
        '#attributes' => array('class' => array('achievement-leaderboard', 'achievement-leaderboard-' . $type)),
        '#header'     => $type == 'top' ? $header : NULL, // relative piggybacks off of top.
        '#empty'      => $type == 'top' ? t('No one has been ranked yet.') : NULL,
        '#rows'       => $rows[$type],
        '#theme'      => 'table',
      ),
    );
    if ($type == 'top' || count($rows['relative'])) {
      drupal_alter('achievements_leaderboard', $leaderboard);
      $build['achievements']['leaderboard'][$type] = $leaderboard['render'];
    }
  }

  $build['achievements']['leaderboard_pager'] = array(
    '#theme'  => 'pager',
  );

  return $build;
}

/**
 * Block callback; show the global leaderboard.
 *
 * @see achievement_leaderboard_totals()
 */
function achievements_leaderboard_block() {
  $header = array(array('data' => t('#'), 'sort' => NULL), t('Who'), t('Pts'));
  $achievers = $rows = array('top' => array(), 'relative' => array());

  $query = db_select('achievement_totals', 'at');
  $query->addTag('achievement_totals')->join('users', 'u', 'u.uid = at.uid');
  $query->fields('at', array('uid', 'points', 'unlocks'))->fields('u', array('name'));
  $query->orderBy('at.points', 'DESC')->orderBy('at.timestamp'); 
  $query->range(0, variable_get('achievements_leaderboard_block_count_top', 5));
  $achievers['top'] = $query->execute()->fetchAllAssoc('uid');

  if (user_is_logged_in() && !isset($achievers['top'][$GLOBALS['user']->uid]) && variable_get('achievements_leaderboard_block_relative', 'nearby_ranks') != 'disabled') {
    $achievers['relative'] = (variable_get('achievements_leaderboard_block_relative', 'nearby_ranks') == 'nearby_ranks')
      ? achievements_totals_user(variable_get('achievements_leaderboard_block_relative_nearby_ranks', 1))
      : achievements_totals_user(); 
  }

  $rank = 0;
  foreach (array('top', 'relative') as $type) {
    foreach ($achievers[$type] as $achiever) {
      $rank = $achiever->rank = isset($achiever->rank) ? $achiever->rank : $rank + 1;
      $classes = ($achiever->uid == $GLOBALS['user']->uid) ? array('achievement-leaderboard-current-user') : array();
      if ($rank <= 3) { 
        $classes[] = 'achievement-leaderboard-top-rank';
        $classes[] = 'achievement-leaderboard-top-rank-' . $rank;
      }

      $rows[$type][$achiever->uid] = array(
        'class' => $classes,
        'data'  => array(
          array(
            'data'  => $rank,
            'class' => array('achievement-leaderboard-rank')
          ),
          array(
            'data'  => theme('username', array('account' => $achiever)),
            'class' => array('achievement-leaderboard-username')
          ),
          array(
            'data'  => l($achiever->points, 'user/' . $achiever->uid . '/achievements'),
            'class' => array('achievement-leaderboard-points')
          ),
        ),
      );
    }
  }

  foreach (array('top', 'relative') as $type) {
    $leaderboard = array(
      'achievers' => $achievers,
      'block'     => TRUE,
      'type'      => $type,
      'render'    => array(
        '#attributes' => array('class' => array('achievement-leaderboard', 'achievement-leaderboard-' . $type)),
        '#header'     => $type == 'top' ? $header : NULL, // relative piggybacks off of top.
        '#empty'      => $type == 'top' ? t('No one has been ranked yet.') : NULL,
        '#rows'       => $rows[$type],
        '#theme'      => 'table',
      ),
    );
    if ($type == 'top' || count($rows['relative'])) {
      drupal_alter('achievements_leaderboard', $leaderboard);
      $build['achievements']['leaderboard'][$type] = $leaderboard['render'];
    }
  }

  $build['achievements']['see-more'] = array(
    '#prefix' => '<div class="achievement-see-more">',
    '#markup' =>  l(t('View full leaderboard »'), 'achievements/leaderboard'),
    '#suffix' => '</div>',
  );

  return $build;
}

/**
 * Menu callback; display a single achievement page with leaderboards.
 */
function achievements_leaderboard_for($achievement) {
  drupal_set_title(t('Achievement: @title', array('@title' => $achievement['title'])), PASS_THROUGH);

  $build['achievements']['achievement'] = array(
    '#achievement'  => $achievement,
    '#unlock'       => achievements_unlocked_already($achievement['id']),
    '#theme'        => 'achievement__leaderboard_for',
  );

  // get stats for first and most recent unlocks.
  $query = db_select('achievement_unlocks', 'au');
  $query->join('achievement_totals', 'at', 'at.uid = au.uid');
  $query->join('users', 'u', 'u.uid = au.uid');
  $query->condition('au.achievement_id', $achievement['id']); 
  $query->fields('au', array('uid', 'rank', 'timestamp'))->fields('at', array('points', 'unlocks'))->fields('u', array('name'));
  $query2 = clone $query; 
  $achievers['first']  = $query->orderBy('rank')->range(0, 10)->execute()->fetchAllAssoc('uid'); 
  $achievers['recent'] = $query2->orderBy('timestamp', 'DESC')->range(0, 10)->execute()->fetchAllAssoc('uid');

  foreach (array('first', 'recent') as $type) {
    $rows = array(); 
    foreach ($achievers[$type] as $achiever) {
      $rows[$achiever->uid] = array(
        'class' => array(),
        'data' => array(
          array(
            'data'  => $achiever->rank,
            'class' => array('achievement-leaderboard-rank')
          ),
          array(
            'data'  => theme('username', array('account' => $achiever)),
            'class' => array('achievement-leaderboard-username')
          ),
          array(
            'data'  => l($achiever->points, 'user/' . $achiever->uid . '/achievements'),
            'class' => array('achievement-leaderboard-points')
          ),
          array(
            'data'  => format_date($achiever->timestamp, 'short'),
            'class' => array('achievement-leaderboard-when')
          ),
        ),
      );
    }

    $leaderboard = array(
      'achievers' => $achievers,
      'block'     => FALSE,
      'type'      => $type,
      'render'    => array(
        '#attributes' => array('class' => array('achievement-leaderboard-' . $type)),
        '#caption'    => t('@type achievement unlocks', array('@type' => drupal_ucfirst($type))),
        '#header'     => array(t('#'), t('Who'), t('Points'), t('When')),
        '#empty'      => t('No one has unlocked this yet. Keep trying!'),
        '#rows'       => $rows,
        '#theme'      => 'table',
      ),
    );
    drupal_alter('achievements_leaderboard', $leaderboard);
    $build['achievements']['leaderboard'][$type] = $leaderboard['render'];
  }

  return $build;
}

/**
 * Menu callback; display all achievements for the passed user.
 *
 * @param $account
 *   The user object this request applies against.
 */
function achievements_user_page($account) {
  drupal_set_title(t('Achievements for @name', array('@name' => format_username($account))));
  $unlocks = achievements_unlocked_already(NULL, $account->uid);
  $achievers = achievements_totals_user(0, $account->uid);
  $achiever = reset($achievers);

  $build['achievements']['stats'] = array(
    '#theme'  => 'achievement_user_stats',
    '#stats'  => array(
      'name'          => format_username($account),
      'rank'          => isset($achiever->rank) ? $achiever->rank : 0,
      'points'        => isset($achiever->points) ? $achiever->points : 0,
      'unlocks_count' => count($unlocks),
      'total_count'   => count(achievements_load()),
    ),
  );

  $achievements_grouped = achievements_load(NULL, TRUE);

  $build['achievements']['tabs']['#theme_wrappers'] = array('achievement_groups_wrapper');

  if (count($achievements_grouped) > 1) {
    foreach ($achievements_grouped as $group_id => $group) {
      $links['achievement-group-' . $group_id] = array(
        'title'     => $group['title'],
        'fragment'  => 'achievement-group-' . $group_id,
        'href'      => '', 
        'external'  => TRUE,
      );
    }
    $build['achievements']['tabs']['navigation'] = array('#theme' => 'links', '#links' => $links);
    $build['achievements']['tabs']['navigation']['#attached']['library'][] = array('system', 'ui.tabs');
    $build['achievements']['tabs']['navigation']['#attached']['js']= array( 
      'jQuery(document).ready(function(){jQuery("#achievement-groups").tabs();});' => array('type' => 'inline')
    );
   }

  foreach ($achievements_grouped as $group_id => $group) {
    $locked_weight = 0; 
    $build['achievements']['tabs']['groups'][$group_id]['#group_id'] = $group_id; 
    $build['achievements']['tabs']['groups'][$group_id]['#theme_wrappers'] = array('achievement_group_wrapper');

    foreach ($group['achievements'] as $achievement_id => $achievement) {
      if (!empty($achievement['invisible']) && !isset($unlocks[$achievement_id])) {
        continue; 
      }

      $build['achievements']['tabs']['groups'][$group_id][$achievement_id]['#achievement'] = $achievement;
      $build['achievements']['tabs']['groups'][$group_id][$achievement_id]['#theme'] = 'achievement__user_page';

      if (isset($unlocks[$achievement_id])) {
        $build['achievements']['tabs']['groups'][$group_id][$achievement_id]['#unlock'] = $unlocks[$achievement_id];

        if (variable_get('achievements_unlocked_move_to_top', TRUE)) {
          $build['achievements']['tabs']['groups'][$group_id][$achievement_id]['#weight'] = -$unlocks[$achievement_id]['timestamp'];
        }
      }
      elseif (!isset($unlocks[$achievement_id]) && variable_get('achievements_unlocked_move_to_top', TRUE)) {
        $build['achievements']['tabs']['groups'][$group_id][$achievement_id]['#weight'] = $locked_weight++;

      }

      if (user_permission('manually grant achievements')) {
        $contextual_links = array(
          'achievements' => array('user', array($account->uid, 'achievements', $achievement_id)),
        );

        $build['achievements']['tabs']['groups'][$group_id][$achievement_id]['#achievement']['#contextual_links'] = $contextual_links;
      }
    }
  }

  return $build;
}

/**
 * Menu callback; Retrieve autocomplete suggestions for achievement names.
 */
function achievements_autocomplete($string = '') {
  $achievements = achievements_load(); 
  array_walk($achievements, 'achievements_autocomplete_search', $string);
  drupal_json_output(array_slice(array_filter($achievements), 0, 10));
}

/**
 * Manually grant achievement.
 *
 * @param object $account
 *   Drupal user account object.
 * @param array $achievement
 *   The achievement definition.
 * @param string $action
 *   Action to perform, either 'grant' or 'take'.
 */
function achievements_confirm($form, &$form_state, $account, $achievement, $action) {
  $form['#achievement'] = $achievement;
  $form['#account'] = $account;
  $form['action'] = array(
    '#type' => 'value',
    '#value' => $action,
  );

  $tokens = array(
    '%achievement' => $achievement['title'],
    '%user' => format_username($account),
  );
  $question = $action == 'grant'
    ? t('Grant %achievement to %user?', $tokens)
    : t('Take %achievement away from %user', $tokens);
  $path = isset($_GET['destination']) ? $_GET['destination'] : 'user/' . $account->uid . '/achievements';
  $yes = t('Confirm');

  return confirm_form($form, $question, $path, '', $yes);
}

/**
 * Grant or take an achivement.
 */
function achievements_confirm_submit($form, &$form_state) {
  $action = $form_state['values']['action'];
  $achievement = $form['#achievement'];
  $account = $form['#account'];

  if (!user_access('earn achievements', $account)) {
    drupal_set_message(t('%username does not have permission to earn achievements.',
      array('%username' => format_username($form['#account']))), 'error');
    return;
  }

  if ($action == 'grant') {
    achievements_unlocked($achievement['id'], $account->uid);
    drupal_set_message(t('%username has been given %achievement.', 
      array('%username' => format_username($account), '%achievement' => $achievement['title'])));
    return;
  }
  elseif ($action == 'take') {
    achievements_locked($achievement['id'], $account->uid);
    drupal_set_message(t('%username has had %achievement taken away.', 
      array('%username' => format_username($account), '%achievement' => $achievement['title'])));
    return;
  }
}

/**
 * array_walk helper function for achievements_autocomplete().
 */
function achievements_autocomplete_search(&$value, $key, $string) {
  $value = (stripos($value['title'], $string) === FALSE) ? FALSE : $value['title'];
}

/**
 * Default theme implementation for a user's achievement stats.
 *
 * @param variables
 *   An associative array containing:
 *   - stats: An array which contains the following keys:
 *     name, rank, points, unlocks_count, and total_count.
 */
function theme_achievement_user_stats($variables) {
  $output = '<div class="achievement-user-stats">'; 
  $output .= t('@name is ranked #@rank with @points points. @unlocks_count of @total_count achievements have been unlocked.', array(
    '@name'           => $variables['stats']['name'],
    '@rank'           => $variables['stats']['rank'],
    '@points'         => $variables['stats']['points'],
    '@unlocks_count'  => $variables['stats']['unlocks_count'],
    '@total_count'    => $variables['stats']['total_count'],
  ));
  $output .= '</div>';
  return $output;
}

/**
 * Default theme for the wrapper around a user's achievements page.
 * @param $variables
 */
function theme_achievement_groups_wrapper($variables) {
  return '<div id="achievement-groups">' . $variables['element']['#children'] . '</div>';
}

/**
 * Default theme for the wrapper around an achievement group.
 * @param $variables
 */
function theme_achievement_group_wrapper($variables) {
  $group_id = 'achievement-group-' . $variables['element']['#group_id'];
  return '<div id="' . $group_id . '" class="achievement-group">' . $variables['element']['#children'] . '</div>';
}
