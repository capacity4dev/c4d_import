<?php
/**
 * @file
 * Base class to import data into capacity4more.
 */

/**
 * Base class to import data from capacity4dev into capacity4more.
 */
abstract class C4dMigrateImportBase extends Migration {
  /**
   * Variable prefix to use to pass files from the row to the migration.
   *
   * @var string
   */
  const FILE_PREFIX_EXTENSION = 'C4D_MIGRATE_FILE_EXTENSION_';
  const FILE_PREFIX_FILE = 'C4D_MIGRATE_FILE_FILE_';
  const FILE_PREFIX_PATH = 'C4D_MIGRATE_FILE_PATH_';
  const FILE_PREFIX_FILEMIME = 'C4D_MIGRATE_FILE_FILEMIME_';

  /**
   * Defined array of filemimes as class const variable.
   *
   * @var array
   *   List of mimetypes (string) of images.
   */
  private static $fileImages = array(
    'image/jpeg',
    'image/gif',
    'image/png',
    'image/svg+xml',
    'image/tiff',
    'image/x-ms-bmp',
  );

  /**
   * Static mapping of groups (source to destination).
   *
   * @var array
   */
  static private $groupMapping;

  /**
   * The search indexes that were disabled when the process started.
   *
   * Process will use these to re-enable them.
   */
  static protected $searchIndexDisabled = array();

  /**
   * General initialization of a Migration object.
   *
   * This implements the common functionality to import nodes from a flat table.
   *
   * @param object $group
   *   The migrate Group object.
   */
  public function __construct($group = NULL) {
    // TODO: find out why we get an array instead of the expected MigrateGroup
    // object.
    if ($group instanceof MigrateGroup) {
      // OK : This is what the interface tells us what it should be!
    }
    elseif (!empty($group['group_name'])) {
      // We get an array and the group name is part of it.
      $group = MigrateGroup::getInstance($group['group_name']);
    }
    else {
      // Fallback if there is no group provided.
      $group = MigrateGroup::getInstance(C4D_MIGRATE_MIGRATION_GROUP);
    }
    parent::__construct($group);
  }

  /**
   * {@inheritdoc}
   */
  protected function beginProcess($new_status) {
    parent::beginProcess($new_status);

    if (module_exists('search_api')) {
      $indexes = search_api_index_load_multiple(FALSE);
      foreach ($indexes as $index) {
        if (!$index->status) {
          // Already disabled.
          continue;
        }

        search_api_index_disable($index->id);
        $this->searchIndexDisabled[] = $index->id;
      }
    }
  }

  /**
   * Re-enable the search indexes.
   *
   * {@inheritDoc}
   */
  public function endProcess() {
    parent::endProcess();

    if (!empty($this->searchIndexDisabled)) {
      foreach ($this->searchIndexDisabled as $index_id) {
        search_api_index_enable($index_id);
      }
    }
  }

  /**
   * Get the connection to the D6 database.
   *
   * @return DatabaseConnection
   *   The Database connection object.
   *
   * @throws Exception
   *   Throws database connection Exception.
   */
  public function getConnection() {
    return c4d_migrate_get_db_connection();
  }

  /**
   * Set the right database and return a selectQuery.
   *
   * @param string $name
   *   Name of the database table.
   * @param string $alias
   *   Alias to give the database table.
   *
   * @return bool|SelectQueryInterface
   *   The select object.
   */
  public function createSelect($name, $alias) {
    if ($conn = $this->getConnection()) {
      return $conn->select($name, $alias);
    }

    return FALSE;
  }

  /**
   * Helper to add mapping for a file.
   *
   * @param string $target
   *   The name of the field were the file needs to be stored.
   * @param string $source
   *   The name of the field that holds the file ID (fid) in the data row from
   *   the source table.
   * @param int $file_exists
   *   What to do when a file already exists with the same name.
   *   Options:
   *   - FILE_EXISTS_RENAME  : (default) Appends number until name is unique.
   *   - FILE_EXISTS_REPLACE : Replace the existing file.
   *   - FILE_EXISTS_ERROR   : Do nothing and return FALSE.
   */
  public function addFileFieldMapping(
    $target,
    $source,
    $file_exists = FILE_EXISTS_RENAME
  ) {
    // File info is passed trough temporary variables.
    $file = self::FILE_PREFIX_FILE . $source;
    $path = self::FILE_PREFIX_PATH . $source;

    FILE_EXISTS_ERROR;

    $this->addFieldMapping($file, $file);
    $this->addFieldMapping($path, $path);
    $this->addFieldMapping($target, $file);
    $this->addFieldMapping($target . ':source_dir', $path);
    $this->addFieldMapping($target . ':file_replace')
      ->defaultValue($file_exists);
  }

  /**
   * Helper to prepare the field for a file field.
   *
   * This will query the file table from the D6 environment to retrieve the file
   * details and add it to the row. The mapping will use this data to fill the
   * file field on the new platform.
   *
   * @param object $row
   *   The data row.
   * @param string $source
   *   The name of the file source field.
   *   This field should contain a file id (fid).
   * @param string $table
   *   The table to use to lookup the file data.
   */
  public function prepareFileInRow(
    $row,
    $source,
    $table = 'c4d_export_other_files'
  ) {
    // File info is passed trough temporary variables.
    $extension = self::FILE_PREFIX_EXTENSION . $source;
    $file = self::FILE_PREFIX_FILE . $source;
    $path = self::FILE_PREFIX_PATH . $source;
    $filemime = self::FILE_PREFIX_FILEMIME . $source;

    // Prepare with empty values.
    $row->{$extension} = NULL;
    $row->{$file} = NULL;
    $row->{$path} = NULL;
    $row->{$filemime} = NULL;

    // Check if any file data in the platform.
    if (empty($row->{$source})) {
      return;
    }

    // Retrieve actual file record from fid.
    $f_select = $this->getConnection()->select($table, 'f');
    $f_select->fields('f');
    $f_select->condition('f.fid', (int) $row->{$source});
    $res = $f_select->execute();
    $result = $res->fetchObject();

    $path_parts = pathinfo($result->filepath);

    // All other keys combined are the relative path from the root dir.
    $rel_path = $path_parts['dirname'];

    // Pass the filename.
    $row->{$extension} = strtolower($path_parts['extension']);
    $row->{$file} = $path_parts['basename'];
    $row->{$filemime} = $result->filemime;
    $row->{$path} = variable_get(C4D_MIGRATE_FILES_ROOT, '/')
      . DIRECTORY_SEPARATOR
      . $rel_path;
  }

  /**
   * Helper function to check if this row's field is an image field.
   *
   * @param object $row
   *   The data from the table.
   * @param string $field_name
   *   The name of the field to check.
   *
   * @return bool
   *   TRUE or FALSE whether it's an image or not.
   */
  protected function isImageField($row, $field_name) {
    $filemime = self::FILE_PREFIX_FILEMIME . $field_name;

    if (isset($row->{$filemime}) && in_array($row->{$filemime}, self::$fileImages)) {
      return TRUE;
    }

    return FALSE;
  }

  /**
   * Retrieve the destination nid of the gid.
   *
   * @param string $type
   *   String containing the group type (machine name).
   * @param int $gid
   *   The nid of the term's group we are importing.
   *
   * @return int
   *   The destination nid of the group.
   */
  protected function getMappedGroupGid($type, $gid) {
    if (empty(self::$groupMapping)) {
      $cache = cache_get('c4d_migrate_og_group_mappings', 'cache');

      if (empty($cache->data)) {
        $mapped_ids = array();
      }
      else {
        $mapped_ids = $cache->data;
      }
    }
    else {
      $mapped_ids = self::$groupMapping;
    }

    $cache_changed = FALSE;

    if (!empty($mapped_ids[$gid])) {
      $dest_id = $mapped_ids[$gid];
    }
    else {
      switch ($type) {
        case 'project_group':
          $dest_id = $this->handleSourceMigration(
            'C4dMigrateImportNodeProject',
            $gid,
            NULL,
            $this
          );
          break;

        case 'group':
        default:
          $dest_id = $this->handleSourceMigration(
            'C4dMigrateImportNodeGroup',
            $gid,
            NULL,
            $this
          );
          break;
      }
      $mapped_ids[$gid] = $dest_id;

      $cache_changed = TRUE;
    }

    if ($cache_changed) {
      // Set temporary cache. If we do a rollback and import again, we have
      // new mappings.
      cache_set(
        'c4d_migrate_og_group_mappings',
        $mapped_ids,
        'cache',
        CACHE_TEMPORARY
      );
    }

    self::$groupMapping = $mapped_ids;

    return $dest_id;
  }

  /**
   * Helper to process a field containing HTML created with WYSIWYG editors.
   *
   * It will search for [tags] and process them into media, html, …
   *
   * @param string $field
   *   The field that needs to be processed.
   *
   * @see C4dMigrateProcessBodyMedia
   *
   * @return string
   *   The processed field.
   */
  protected function prepareHtml($field) {
    static $processor;

    if (!$processor) {
      $processor = new C4dMigrateProcessBodyMedia();
      $processor->addLogger(new C4dMigrateLoggerDrush());
      $processor->addLogger(new C4dMigrateLoggerMigrate());
    }

    return $processor->process($field);
  }

  /**
   * Map terms to the existing terms in the database for a specified taxonomy.
   *
   * Map terms to the existing terms in the database for a specified taxonomy.
   * Existing terms are provided by an install hook.
   *
   * @param array $sourcefield
   *   Array of terms from the sourcefield.
   * @param string $vocabname
   *   Name of the vocabulary.
   *
   * @return array
   *   Array of new term ids.
   */
  public function mapTaxonomyVocabRegionCountries($sourcefield, $vocabname) {
    $vocab = taxonomy_vocabulary_machine_name_load($vocabname);

    if (!empty($vocab)) {
      $terms = $sourcefield;
      if (!empty($terms)) {
        $tids = array();
        $mapping = $this->getMappingRegions();

        foreach ($terms as $term_name) {
          $term = taxonomy_get_term_by_name($term_name, $vocabname);

          // Case 1: Term is available in the new platform,
          // with the exact same name.
          if (!empty($term)) {
            reset($term);
            $tid = key($term);
            $tids[] = $tid;
          }
          else {
            if (!empty($mapping[$term_name])) {
              $mapped_term = taxonomy_get_term_by_name(
                $mapping[$term_name],
                $vocabname
              );
              if (!empty($mapped_term)) {
                reset($mapped_term);
                $tid = key($mapped_term);
                $tids[] = $tid;
              }
            }
          }
        }

        return $tids;
      }
    }

    return array();
  }


  /**
   * Retrieve mappings between terms and their parents.
   *
   * @param array $tids
   *   Array of term ids content is tagged with.
   *
   * @return array
   *   Array of term ids, including every term's parents.
   */
  public function mapTaxonomyVocabWithParent($tids) {
    $all_tids = array();

    if (empty($tids)) {
      return array();
    }

    foreach ($tids as $tid) {
      $parents = taxonomy_get_parents_all($tid);
      $parent_tids = array_map(
        create_function('$term', 'return $term->tid;'),
        $parents
      );
      // $tid is included in $parent_tids array.
      $all_tids = array_merge($all_tids, $parent_tids);
    }

    $unique = array_unique($all_tids);
    sort($unique);

    return $unique;
  }

  /**
   * Retrieve an array of mapped region values.
   *
   * @return array
   *   Mapping between old and new region naming.
   */
  private function getMappingRegions() {
    return array(
      '1.Sub-Saharan Africa' => 'Sub-Saharan Africa',
      'Sub-Saharan Africa' => 'Sub-Saharan Africa',
      '2.European Neighbourhood & Russia' => 'Europe & Russia',
      'European Neighbourhood & Russia' => 'Europe & Russia',
      '3.Latin America & Caribbean' => 'Latin America & Caribbean',
      'Latin America & Caribbean' => 'Latin America & Caribbean',
      // Splits into different regions in the new platform?
      // Asia vs Pacific (subregion of Oceania & Pacific) vs Gulf, not mapped.
      /*'4.Asia, Gulf Region & Pacific' => '',
      'Asia, Gulf Region & Pacific' => '',*/
      '5.EU candidate and potential candidate countries' => 'EU Candidate & Potential candidate countries',
      'EU candidate and potential candidate countries' => 'EU Candidate & Potential candidate countries',
      '6.OCTs (Overseas Countries and Territories)' => 'OCTs (Overseas Countries & Territories)',
      'OCTs (Overseas Countries and Territories)' => 'OCTs (Overseas Countries & Territories)',
      '7.EU Member States' => 'EU Member States',
      'EU Member States' => 'EU Member States',
      // Splits into multiple (sub)regions in the new platform,
      // which don't match.
      /*'8.Other Countries' => '',
      'Other Countries' => '',*/
    );
  }

  /**
   * Retrieve the title of the node in the old platform based on the nid.
   *
   * @param int $nid
   *   Drupal node id (source).
   */
  protected function getNodeTitleFromNid($nid) {
    $title = $this->createSelect('node', 'n')
      ->fields('n', array('title'))
      ->condition('nid', $nid)
      ->execute()
      ->fetchField();
    return $title;
  }

  /**
   * Retrieves the entry in the Categories & Tags mapping array
   * with the given term name.
   *
   * @param string $termName
   *   The term name to search for.
   *
   * @return bool|array
   *   Returns the value of the mapping item with the given term name or
   *   FALSE if none is found.
   */
  public function mappingCategoriesAndTagsContainsTermName($termName) {
    $termName = htmlspecialchars_decode($termName);
    $mapping = $this->getMappingCategoriesAndTags();

    if (isset($mapping[$termName]) && !empty($mapping[$termName])) {
      return $mapping[$termName];
    }

    return FALSE;
  }

  /**
   * Retrieve the Categories & Tags mapping array.
   */
  public function getMappingCategoriesAndTags() {
    if (is_null($this->mappingCategoriesAndTags)) {
      $mapping = c4d_migrate_categories_tags_csv_converter_get_mapping_array();
      $this->mappingCategoriesAndTags = $mapping;
    }

    if (empty($this->mappingCategoriesAndTags)) {
      $this->mappingCategoriesAndTags = array();
    }

    return $this->mappingCategoriesAndTags;
  }

}
