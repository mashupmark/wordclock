<script lang="ts" setup>
import * as v from "valibot";
import type { FormSubmitEvent } from "#ui/types";

const toast = useToast();

const schema = v.object({
  SSID: v.pipe(v.string(), v.trim(), v.nonEmpty("required")),
  PW: v.optional(v.pipe(v.string(), v.trim())),
});
type Schema = v.InferOutput<typeof schema>;

const state = reactive<Schema>({ SSID: "" });

const onSubmit = async (data: FormSubmitEvent<Schema>) => {
  try {
    await $fetch("/api/wifi", { method: "PUT", body: data });
  } catch {
    toast.add({
      title: "Error",
      description: "Failed to update wifi credentials",
      color: "red",
    });
  }
};
</script>

<template>
  <UForm
    class="flex flex-col gap-2 p-4 border-b border-gray-200 dark:border-gray-800"
    :schema="v.safeParser(schema)"
    :state
    @submit="onSubmit"
  >
    <UFormGroup
      label="Wifi"
      description="Update the wifi credentials used to connect to the internet"
      size="xl"
      class="pb-4 flex justify-between border-b border-gray-200 dark:border-gray-800"
      :ui="{ label: { base: 'font-semibold' } }"
    >
      <UButton label="Save changes" type="submit" color="black" />
    </UFormGroup>

    <UFormGroup label="SSID" name="SSID" required>
      <UInput v-model="state.SSID" />
    </UFormGroup>
    <UFormGroup label="Password" name="PW">
      <UInput
        v-model="state.PW"
        type="password"
        trailing-icon="i-heroicons-lock-closed"
      />
    </UFormGroup>
  </UForm>
</template>
