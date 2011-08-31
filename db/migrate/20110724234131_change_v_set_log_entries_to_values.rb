class ChangeVSetLogEntriesToValues < ActiveRecord::Migration
  def self.up
    rename_column :set_log_entries, :v1, :weight
    rename_column :set_log_entries, :v2, :reps
  end

  def self.down
  end
end
