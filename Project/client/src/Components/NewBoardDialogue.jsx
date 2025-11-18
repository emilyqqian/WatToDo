import { useState } from 'react'
import {
  Button,
  Dialog,
  DialogActions,
  DialogContent,
  DialogTitle,
  FormControl,
  InputLabel,
  MenuItem,
  Select,
  Stack,
  TextField,
} from '@mui/material'

export default function TaskboardDialog({ open, onClose, onSave }) {
  const [boardName, setBoardName] = useState('')
  const [boardType, setBoardType] = useState('personal')

  const handleClose = () => {
    setBoardName('')
    setBoardType('personal')
    onClose()
  }

  const handleSave = () => {
    if (!boardName.trim()) return
    onSave({ name: boardName.trim(), type: boardType })
    handleClose()
  }

  return (
    <Dialog open={open} onClose={handleClose} fullWidth maxWidth="sm">
      <DialogTitle>New Task Board</DialogTitle>
      <DialogContent>
        <Stack spacing={3} sx={{ pt: 1 }}>
          <TextField
            label="Board Name"
            value={boardName}
            onChange={(event) => setBoardName(event.target.value)}
            fullWidth
          />
          <FormControl fullWidth>
            <InputLabel id="board-type-label">Board Type</InputLabel>
            <Select
              labelId="board-type-label"
              label="Board Type"
              value={boardType}
              onChange={(event) => setBoardType(event.target.value)}
            >
              <MenuItem value="personal">Personal</MenuItem>
              <MenuItem value="shared">Shared</MenuItem>
            </Select>
          </FormControl>
        </Stack>
      </DialogContent>
      <DialogActions sx={{ px: 3, pb: 2 }}>
        <Button onClick={handleClose}>Cancel</Button>
        <Button variant="contained" onClick={handleSave}>
          Save Board
        </Button>
      </DialogActions>
    </Dialog>
  )
}