<script lang="ts" setup>
import * as v from "valibot";
import SettingsSection from "./SettingsSection.vue";

const schema = v.object({
  color: v.pipe(v.string(), v.hexColor()),
});
type Schema = v.InferOutput<typeof schema>;

const state = reactive<Schema>({ color: "#ff0000" });
</script>

<template>
  <SettingsSection
    title="Clock Color"
    description="Set the color the time is displayed in"
    :schema
    :state
  >
    <UFormGroup label="Color" name="color" required>
      <UInput
        v-model="state.color"
        type="color"
        variant="none"
        :padded="false"
      />
    </UFormGroup>
  </SettingsSection>
</template>
