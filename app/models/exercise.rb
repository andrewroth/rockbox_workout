class Exercise < ActiveRecord::Base
  include Csv

  belongs_to :exercise_type
  has_many :exercise_sets

  def name_with_type
    "#{name} (#{exercise_type.try(:name)})"
  end
end
