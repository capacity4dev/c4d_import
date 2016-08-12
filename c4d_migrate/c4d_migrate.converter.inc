<?php

/**
 * C4D Migrate - Categories & Tags CSV converter.
 */

/**
 * Page callback for /admin/content/migrate/categories-tags/csv-converter
 */
function c4d_migrate_categories_tags_csv_converter() {
  $mapping = c4d_migrate_categories_tags_csv_converter_get_mapping_array();

  if (function_exists('dpm')) {
    dpm($mapping, 'Mapping array');
  }
  else {
    var_dump($mapping);
  }

  return '';
}

/**
 * Returns the mapping array with all taxonomy term information.
 *
 * @return array $mapping
 *   The mapping array with all the taxonomy term information.
 */
function c4d_migrate_categories_tags_csv_converter_get_mapping_array() {
  $mapping = &drupal_static(__FUNCTION__);

  if (!isset($mapping)) {
    $mapping = array();

    // Dates.
    _c4d_migrate_categories_tags_csv_converter_helper($mapping, 'dates.csv', 'c4m_vocab_date');
    // Document types.
    _c4d_migrate_categories_tags_csv_converter_helper($mapping, 'document-types.csv', 'c4m_vocab_document_type');
    // Languages.
    _c4d_migrate_categories_tags_csv_converter_helper($mapping, 'languages.csv', 'c4m_vocab_language');
    // Regions & countries.
    _c4d_migrate_categories_tags_csv_converter_helper($mapping, 'regions_countries.csv', 'c4m_vocab_geo');
    // Topics.
    _c4d_migrate_categories_tags_csv_converter_helper($mapping, 'topics.csv', 'c4m_vocab_topic');
  }

  return $mapping;
}

/**
 * Helper function in order to add CSV data to a mapping array.
 *
 * @param array $mapping
 *   Mapping array.
 * @param $filename
 *   The filename of the CSV file in the folder "[c4d_migrate-module]/includes/csv/categories_tags".
 * @param $vocabulary
 *   The vocabulary the entries are related to.
 *
 * @return bool
 *   In case the file cannot be found, FALSE is returned, otherwise TRUE.
 */
function _c4d_migrate_categories_tags_csv_converter_helper(&$mapping, $filename, $vocabulary) {
  $filepath = drupal_get_path('module', 'c4d_migrate') . '/includes/csv/categories_tags/' . $filename;

  if(!file_exists($filepath) || !is_readable($filepath)) {
    return FALSE;
  }

  $file = fopen($filepath, 'r');

  while (!feof($file)) {
    $row = fgetcsv($file, 0, ';');

    if (isset($row[0]) && !empty($row[0])) {
      $key = trim($row[0]);

      $mapping[$key] = array(
        'vocabulary' => $vocabulary,
        'terms'      => array(),
      );

      for ($i = 1; $i < sizeof($row); $i++) {
        if (isset($row[$i]) && !empty($row[$i])) {
          $mapping[$key]['terms'][] = trim($row[$i]);
        }
      }
    }
  }

  fclose($file);

  return TRUE;
}
