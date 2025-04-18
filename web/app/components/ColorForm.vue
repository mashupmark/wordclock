<script lang="ts" setup>
import * as v from "valibot";
import SettingsSection from "./SettingsSection.vue";

const toast = useToast();

const schema = v.object({
  color: v.pipe(v.string(), v.hexColor()),
});
type Schema = v.InferOutput<typeof schema>;

const state = reactive<Schema>({ color: "#ff0000" });

const { status: saveStatus, execute: save } = useAsyncData(
  async () => {
    try {
      await $fetch("/api/settings/color", {
        method: "PUT",
        body: { color: state.color },
      });
    } catch {
      toast.add({ title: "Failed to update color", color: "red" });
    }
  },
  { immediate: false }
);
</script>

<template>
  <SettingsSection
    title="Clock Color"
    description="Set the color the time is displayed in"
    :schema
    :state
    :isSaving="saveStatus === 'pending'"
    @submit="save()"
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
