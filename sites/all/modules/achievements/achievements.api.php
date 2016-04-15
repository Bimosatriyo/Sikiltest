<?php

/**
 * @file
 */

/**
 * Define an achievement.
 */
function hook_achievements_info() {
  $achievements = array(
    'comment-count-50' => array(
      'title'       => t('Posted 50 comments!'),
      'description' => t("We no longer think you're a spam bot. Maybe."),
      'storage'     => 'comment-count',
      'points'      => 50,
    ),
    'comment-count-100' => array(
      'title'       => t('Posted 100 comments!'),
      'description' => t('But what about the children?!'),
      'storage'     => 'comment-count',
      'points'      => 100,
      'images' => array(
        'unlocked'  => 'sites/default/files/example1.png',
      ),
    ),

    'article-creation' => array(
      'title' => t('Article creation'),
      'achievements' => array(
        'node-mondays' => array(
          'title'       => t('Published some content on a Monday'),
          'description' => t("Go back to bed: it's still the weekend!"),
          'points'      => 5,
          'images' => array(
            'unlocked'  => 'sites/default/files/example1.png',
            'locked'    => 'sites/default/files/example2.png',
            'secret'    => 'sites/default/files/example3.png',
          ),
        ),
      ),
    ),
  );

  return $achievements;
}

/**
 * Implements hook_comment_insert().
 */
function example_comment_insert($comment) {
  $current_count = achievements_storage_get('comment-count', $comment->uid) + 1;
  achievements_storage_set('comment-count', $current_count, $comment->uid);

  foreach (array(50, 100) as $count) {
    if ($current_count == $count) {
      achievements_unlocked('comment-count-' . $count, $comment->uid);
    }
  }
}

/**
 * Implements hook_node_insert().
 */
function example_node_insert($node) {
  if (format_date(REQUEST_TIME, 'custom', 'D') == 'Mon') {
    achievements_unlocked('node-mondays', $node->uid);
  }
}

/**
 * Implements hook_achievements_info_alter().
 * @param &$achievements
 */
function example_achievements_info_alter(&$achievements) {
  $achievements['comment-count-100']['points'] = 200;
}

/**
 * Implements hook_achievements_unlocked().
 * @param $achievement
 * @param $uid
 */
function example_achievements_unlocked($achievement, $uid) {
}

/**
 * Implements hook_achievements_locked().
 * @param $achievement
 * @param $uid
 */
function example_achievements_locked($achievement, $uid) {
}

/**
 * Implements hook_achievements_leaderboard_alter().
 * @param &$leaderboard
 */
function example_achievements_leaderboard_alter(&$leaderboard) {
  if ($leaderboard['type'] == 'first') {
    $leaderboard['render']['#caption'] = t('Congratulations to our first 10!');
  }
}

/**
 * Implements hook_query_alter().
 * achievement_totals:
 * achievement_totals_user:
 * achievement_totals_user_nearby:
 */
function example_query_alter(QueryAlterableInterface $query) {
}

/**
 * Implements hook_achievements_access_earn().
 * @param $uid
 * @return
 */
function example_achievements_access_earn($uid) {
  $account = user_load($uid);
  if (format_username($account) == 'Morbus Iff') {
    return TRUE;
  }
}
