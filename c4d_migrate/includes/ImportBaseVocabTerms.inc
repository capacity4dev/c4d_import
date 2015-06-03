<?php
/**
 * @file
 * Migrate class to import Vocabulary Tags.
 */

abstract class C4dMigrateImportBaseVocabTerms extends C4dMigrateImportBase {
  /**
   * Complete function to set missing aliases.
   *
   * @param object $user
   * @param object $row
   */
  public function complete($term, $row) {
    $query = $this->createSelect('url_alias', 'u')
      ->orderBy('src', 'ASC')
      ->fields('u')
      ->condition('src', 'taxonomy/term/' . $row->tid)
      ->execute();
    $results = $query->fetchAll();

    foreach($results as $result) {
      $redirect = new stdClass();

      redirect_object_prepare(
        $redirect,
        array(
          'source' => $result->dst,
          'source_options' => array(),
          'redirect' => 'taxonomy/term//' . $term->tid,
          'redirect_options' => array(),
          'language' => LANGUAGE_NONE,
        )
      );

      redirect_save($redirect);
    }
  }
}