<?php
/**
 * @file
 * Migrate class to import Vocabulary Tags.
 */

/**
 * Base class to import vocab data (terms) from capacity4dev into capacity4more.
 */
abstract class C4dMigrateImportBaseVocabTerms extends C4dMigrateImportBase {
  /**
   * Complete function to set missing aliases.
   *
   * @param object $term
   *   Drupal vocabulary term object.
   * @param object $row
   *   Migration source row object.
   */
  public function complete($term, $row) {
    $query = $this->createSelect('url_alias', 'u')
      ->orderBy('src', 'ASC')
      ->fields('u', array('pid', 'src', 'dst'))
      ->condition('src', 'taxonomy/term/' . $row->tid)
      ->groupBy('dst')
      ->groupBy('src')
      ->groupBy('pid')
      ->execute();
    $results = $query->fetchAll();

    foreach ($results as $result) {
      $redirect = new stdClass();

      redirect_object_prepare(
        $redirect,
        array(
          'source' => $result->dst,
          'source_options' => array(),
          'redirect' => 'taxonomy/term/' . $term->tid,
          'redirect_options' => array(),
          'language' => LANGUAGE_NONE,
        )
      );

      $rid = redirect_load_by_hash(redirect_hash($redirect));
      if (empty($rid)) {
        redirect_save($redirect);
      }
    }
  }

  /**
   * Review a data row after fetch, returning FALSE to skip it.
   *
   * @param object $row
   *   The data row we are importing.
   *
   * @return bool
   *   TRUE to process this row, FALSE to skip it.
   */
  public function prepareRow($row) {
    parent::prepareRow($row);

    // Don't create it if this term name is meant to be mapped to a global term.
    if ($mapping = $this->mappingCategoriesAndTagsContainsTermName($row->name)) {
      return FALSE;
    }

    return TRUE;
  }

}
