class MakeFunctionsLikeSetFunctions < ActiveRecord::Migration
  def self.up
    change_table :functions do |t|
      t.string :variable
      t.integer :exercise_set_id
    end
  end

  def self.down
    remove_column :functions, :variable
    remove_column :functions, :exercise_set_id
  end
end
