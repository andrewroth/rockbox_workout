class AddDescriptions < ActiveRecord::Migration
  def self.up
    add_column :exercises, :description, :text
    add_column :workouts, :description, :text
  end

  def self.down
    remove_column :exercises, :description
    remove_column :workouts, :description
  end
end
